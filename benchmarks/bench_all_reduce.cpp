#include "common/bench_util.hpp"
#include "cpu_collectives/cpu_collectives.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <latch>
#include <thread>
#include <vector>

namespace {

bool parse_positive(const char* text, std::size_t& value) {
    if (text == nullptr || *text == '\0' || *text == '-') {
        return false;
    }
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(text, &end, 10);
    if (*end != '\0' || parsed == 0 || parsed > static_cast<unsigned long long>(SIZE_MAX)) {
        return false;
    }
    value = static_cast<std::size_t>(parsed);
    return true;
}

int run_trial(std::size_t participants, std::size_t elements, std::size_t iterations,
              std::size_t trial, cc_communicator_t* comm) {
    std::vector<std::vector<double>> inputs(participants, std::vector<double>(elements));
    std::vector<std::vector<double>> outputs(participants, std::vector<double>(elements, 0.0));
    for (std::size_t rank = 0; rank < participants; ++rank) {
        std::fill(inputs[rank].begin(), inputs[rank].end(), static_cast<double>(rank + 1));
    }

    std::atomic<bool> failed{false};
    std::latch ready(static_cast<std::ptrdiff_t>(participants));
    std::atomic<bool> go{false};
    std::vector<std::thread> workers;
    workers.reserve(participants);
    const std::size_t warmup_iterations = std::min<std::size_t>(iterations, 100);

    for (std::size_t rank = 0; rank < participants; ++rank) {
        workers.emplace_back([&, rank] {
            for (std::size_t i = 0; i < warmup_iterations; ++i) {
                if (cc_all_reduce_sum_f64(comm, rank, inputs[rank].data(),
                                          outputs[rank].data(), elements) != CC_STATUS_OK) {
                    failed.store(true, std::memory_order_relaxed);
                    return;
                }
            }
            ready.count_down();
            while (!go.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            for (std::size_t i = 0; i < iterations; ++i) {
                if (cc_all_reduce_sum_f64(comm, rank, inputs[rank].data(),
                                          outputs[rank].data(), elements) != CC_STATUS_OK) {
                    failed.store(true, std::memory_order_relaxed);
                    return;
                }
            }
        });
    }

    ready.wait();
    const auto start = cc_bench::Clock::now();
    go.store(true, std::memory_order_release);
    for (auto& worker : workers) {
        worker.join();
    }
    const auto elapsed = cc_bench::Clock::now() - start;

    if (failed.load(std::memory_order_relaxed)) {
        std::cerr << "all-reduce failed during trial " << trial << '\n';
        return 1;
    }

    const double expected = static_cast<double>(participants) *
                            static_cast<double>(participants + 1) / 2.0;
    for (std::size_t rank = 0; rank < participants; ++rank) {
        for (std::size_t i = 0; i < elements; ++i) {
            if (!std::isfinite(outputs[rank][i]) || outputs[rank][i] != expected) {
                std::cerr << "incorrect result in trial " << trial << ", rank " << rank
                          << ", element " << i << ": expected " << expected
                          << ", got " << outputs[rank][i] << '\n';
                return 1;
            }
        }
    }

    const double seconds = std::chrono::duration<double>(elapsed).count();
    const double calls = static_cast<double>(iterations);
    const double average_us = seconds * 1.0e6 / calls;
    const double per_rank_mib_s = (static_cast<double>(elements) * sizeof(double) * calls /
                                   seconds) / (1024.0 * 1024.0);
    std::cout << "all_reduce_sum_f64"
              << " trial=" << trial
              << " participants=" << participants
              << " elements=" << elements
              << " payload_bytes_per_rank=" << elements * sizeof(double)
              << " iterations=" << iterations
              << " elapsed_ms=" << std::chrono::duration<double, std::milli>(elapsed).count()
              << " average_us_per_collective=" << average_us
              << " input_mib_s_per_rank=" << per_rank_mib_s << '\n';
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 5) {
        std::cerr << "usage: bench_all_reduce [participants] [elements] [iterations] [trials]\n";
        return 2;
    }

    std::size_t participants = 2;
    std::size_t elements = 1024;
    std::size_t iterations = 1000;
    std::size_t trials = 5;
    if ((argc > 1 && !parse_positive(argv[1], participants)) ||
        (argc > 2 && !parse_positive(argv[2], elements)) ||
        (argc > 3 && !parse_positive(argv[3], iterations)) ||
        (argc > 4 && !parse_positive(argv[4], trials))) {
        std::cerr << "all arguments must be positive integers\n"
                  << "usage: bench_all_reduce [participants] [elements] [iterations] [trials]\n";
        return 2;
    }

    cc_communicator_t* comm = nullptr;
    const cc_status_t status = cc_communicator_create(participants, &comm);
    if (status != CC_STATUS_OK) {
        std::cerr << "communicator creation failed: " << cc_status_string(status) << '\n';
        return 1;
    }

    std::cout << "Benchmark uses " << std::min<std::size_t>(iterations, 100)
              << " warmup collectives per rank; timing excludes thread creation.\n";
    for (std::size_t trial = 1; trial <= trials; ++trial) {
        if (run_trial(participants, elements, iterations, trial, comm) != 0) {
            cc_communicator_destroy(comm);
            return 1;
        }
    }

    cc_communicator_destroy(comm);
    return 0;
}
