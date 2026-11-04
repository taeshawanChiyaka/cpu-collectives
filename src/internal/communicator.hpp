#pragma once

#include "../sync/barrier.hpp"

#include <atomic>
#include <cstddef>
#include <vector>

namespace cpu_collectives::internal {

enum class CollectiveResult {
    success,
    invalid_argument,
    out_of_memory
};

class Communicator {
public:
    explicit Communicator(std::size_t participant_count)
        : participant_count_(participant_count), barrier_(participant_count),
          input_slots_(participant_count) {}

    [[nodiscard]] std::size_t size() const noexcept { return participant_count_; }
    void barrier() { barrier_.wait(); }

    CollectiveResult all_reduce_sum_f64(std::size_t rank, const double* sendbuf,
                                        double* recvbuf, std::size_t count);

private:
    const std::size_t participant_count_;
    sync::Barrier barrier_;

    // Each participant writes only its own slot. The result is produced by
    // rank 0 and read only after the barrier publishes the completed sum.
    std::vector<std::vector<double>> input_slots_;
    std::vector<double> result_;
    std::size_t collective_count_ = 0;
    std::atomic<bool> allocation_failed_{false};
    std::atomic<bool> invalid_argument_{false};
};

} // namespace cpu_collectives::internal
