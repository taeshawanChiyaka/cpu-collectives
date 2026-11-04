#include "common/bench_util.hpp"
#include "cpu_collectives/cpu_collectives.h"

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    const long participants_arg = argc > 1 ? std::strtol(argv[1], nullptr, 10) : 2;
    const long iterations_arg = argc > 2 ? std::strtol(argv[2], nullptr, 10) : 10000;
    if (participants_arg <= 0 || iterations_arg <= 0) {
        std::cerr << "usage: bench_barrier [positive_participants] [positive_iterations]\n";
        return 2;
    }
    const auto participants = static_cast<std::size_t>(participants_arg);
    const auto iterations = static_cast<std::size_t>(iterations_arg);

    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(participants, &comm) != CC_STATUS_OK) {
        std::cerr << "communicator creation failed\n";
        return 1;
    }

    std::atomic<int> failures{0};
    std::vector<std::thread> workers;
    workers.reserve(participants);
    const auto start = cc_bench::Clock::now();
    for (std::size_t rank = 0; rank < participants; ++rank) {
        workers.emplace_back([&, rank] {
            for (std::size_t i = 0; i < iterations; ++i) {
                if (cc_barrier(comm, rank) != CC_STATUS_OK) {
                    ++failures;
                    return;
                }
            }
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    const auto elapsed = cc_bench::Clock::now() - start;
    cc_communicator_destroy(comm);

    if (failures != 0) {
        std::cerr << "barrier benchmark encountered an API error\n";
        return 1;
    }
    cc_bench::report("condition_variable_barrier", participants, iterations, elapsed);
    return 0;
}
