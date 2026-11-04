#include "cpu_collectives/cpu_collectives.h"

#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

namespace {
int run_sum_test(std::size_t participants, std::size_t count, bool in_place) {
    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(participants, &comm) != CC_STATUS_OK) {
        std::cerr << "communicator creation failed\n";
        return 1;
    }

    std::vector<std::vector<double>> inputs(participants, std::vector<double>(count));
    std::vector<std::vector<double>> outputs(participants, std::vector<double>(count, -1.0));
    std::vector<cc_status_t> statuses(participants, CC_STATUS_INTERNAL_ERROR);
    for (std::size_t rank = 0; rank < participants; ++rank) {
        for (std::size_t i = 0; i < count; ++i) {
            inputs[rank][i] = static_cast<double>((rank + 1) * (i + 1));
        }
    }

    std::vector<std::thread> workers;
    for (std::size_t rank = 0; rank < participants; ++rank) {
        workers.emplace_back([&, rank] {
            double* output = in_place ? inputs[rank].data() : outputs[rank].data();
            statuses[rank] = cc_all_reduce_sum_f64(
                comm, rank, inputs[rank].data(), output, count);
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    bool failed = false;
    for (std::size_t rank = 0; rank < participants; ++rank) {
        if (statuses[rank] != CC_STATUS_OK) {
            std::cerr << "all-reduce failed for rank " << rank << '\n';
            failed = true;
            continue;
        }
        const auto& output = in_place ? inputs[rank] : outputs[rank];
        for (std::size_t i = 0; i < count; ++i) {
            const double expected = static_cast<double>(i + 1) *
                                    static_cast<double>(participants * (participants + 1) / 2);
            if (std::abs(output[i] - expected) > 1e-12) {
                std::cerr << "incorrect sum at rank " << rank << ", index " << i
                          << ": expected " << expected << ", got " << output[i] << '\n';
                failed = true;
            }
        }
    }
    cc_communicator_destroy(comm);
    return failed ? 1 : 0;
}

int run_invalid_argument_test(bool count_mismatch) {
    constexpr std::size_t participants = 2;
    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(participants, &comm) != CC_STATUS_OK) {
        return 1;
    }
    double input = 1.0;
    double output = 0.0;
    cc_status_t statuses[participants] = {CC_STATUS_INTERNAL_ERROR, CC_STATUS_INTERNAL_ERROR};
    std::thread first([&] {
        statuses[0] = cc_all_reduce_sum_f64(comm, 0, &input, &output, 1);
    });
    std::thread second([&] {
        statuses[1] = cc_all_reduce_sum_f64(
            comm, 1, &input, &output, count_mismatch ? 2 : 1);
    });
    first.join();
    second.join();
    cc_communicator_destroy(comm);

    if (statuses[0] != CC_STATUS_INVALID_ARGUMENT ||
        statuses[1] != CC_STATUS_INVALID_ARGUMENT) {
        std::cerr << "collective invalid arguments were not reported to all ranks\n";
        return 1;
    }
    return 0;
}
} // namespace

int main() {
    if (run_sum_test(1, 0, false) != 0 ||
        run_sum_test(2, 1, false) != 0 ||
        run_sum_test(4, 257, false) != 0 ||
        run_sum_test(3, 17, true) != 0 ||
        run_invalid_argument_test(false) != 0 ||
        run_invalid_argument_test(true) != 0) {
        return 1;
    }
    return 0;
}
