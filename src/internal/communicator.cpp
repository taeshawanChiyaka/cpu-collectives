#include "internal/communicator.hpp"

#include <algorithm>
#include <new>

namespace cpu_collectives::internal {

CollectiveResult Communicator::all_reduce_sum_f64(std::size_t rank,
                                                   const double* sendbuf,
                                                   double* recvbuf,
                                                   std::size_t count) {
    // Rank 0 initializes shared state before participants validate and stage
    // their input. The first barrier makes this initialization visible.
    if (rank == 0) {
        allocation_failed_.store(false, std::memory_order_relaxed);
        invalid_argument_.store(false, std::memory_order_relaxed);
        collective_count_ = count;
    }
    barrier_.wait();

    if (count != collective_count_ || (count != 0 && (sendbuf == nullptr || recvbuf == nullptr))) {
        invalid_argument_.store(true, std::memory_order_relaxed);
    } else {
        try {
            input_slots_[rank].resize(count);
            if (count != 0) {
                std::copy_n(sendbuf, count, input_slots_[rank].begin());
            }
            if (rank == 0) {
                result_.resize(count);
            }
        } catch (const std::bad_alloc&) {
            allocation_failed_.store(true, std::memory_order_relaxed);
        } catch (...) {
            // Includes vector length errors. Report an allocation failure
            // collectively instead of allowing one rank to exit early.
            allocation_failed_.store(true, std::memory_order_relaxed);
        }
    }
    barrier_.wait();

    // Snapshot the shared outcome before allowing rank 0 to reset it for a
    // subsequent collective call.
    const bool invalid = invalid_argument_.load(std::memory_order_relaxed);
    const bool out_of_memory = allocation_failed_.load(std::memory_order_relaxed);
    barrier_.wait();
    if (invalid) {
        return CollectiveResult::invalid_argument;
    }
    if (out_of_memory) {
        return CollectiveResult::out_of_memory;
    }

    if (rank == 0) {
        std::fill(result_.begin(), result_.end(), 0.0);
        for (const auto& input : input_slots_) {
            for (std::size_t i = 0; i < count; ++i) {
                result_[i] += input[i];
            }
        }
    }
    barrier_.wait();

    if (count != 0) {
        std::copy_n(result_.begin(), count, recvbuf);
    }
    // Keep result_ and input_slots_ stable until every rank has finished
    // copying; this also makes immediate successive collectives safe.
    barrier_.wait();
    return CollectiveResult::success;
}

} // namespace cpu_collectives::internal
