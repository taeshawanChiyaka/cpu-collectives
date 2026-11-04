#include "internal/communicator.hpp"

#include <algorithm>
#include <new>

namespace cpu_collectives::internal {

CollectiveResult Communicator::all_reduce_sum_f64(std::size_t rank,
                                                   const double* sendbuf,
                                                   double* recvbuf,
                                                   std::size_t count,
                                                   AllReduceAlgorithm algorithm) {
    if (rank == 0) {
        allocation_failed_.store(false, std::memory_order_relaxed);
        invalid_argument_.store(false, std::memory_order_relaxed);
        collective_count_ = count;
        collective_algorithm_ = algorithm;
    }
    barrier_.wait();

    if (count != collective_count_ || algorithm != collective_algorithm_ ||
        (count != 0 && (sendbuf == nullptr || recvbuf == nullptr))) {
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
            allocation_failed_.store(true, std::memory_order_relaxed);
        }
    }
    barrier_.wait();

    const bool invalid = invalid_argument_.load(std::memory_order_relaxed);
    const bool out_of_memory = allocation_failed_.load(std::memory_order_relaxed);
    barrier_.wait();
    if (invalid) {
        return CollectiveResult::invalid_argument;
    }
    if (out_of_memory) {
        return CollectiveResult::out_of_memory;
    }

    if (algorithm == AllReduceAlgorithm::baseline) {
        if (rank == 0) {
            std::fill(result_.begin(), result_.end(), 0.0);
            for (const auto& input : input_slots_) {
                for (std::size_t i = 0; i < count; ++i) {
                    result_[i] += input[i];
                }
            }
        }
    } else {
        const std::size_t base = count / participant_count_;
        const std::size_t remainder = count % participant_count_;
        const std::size_t begin = rank * base + std::min(rank, remainder);
        const std::size_t length = base + (rank < remainder ? 1 : 0);
        const std::size_t end = begin + length;
        for (std::size_t i = begin; i < end; ++i) {
            double sum = 0.0;
            for (const auto& input : input_slots_) {
                sum += input[i];
            }
            result_[i] = sum;
        }
    }
    barrier_.wait();

    if (count != 0) {
        std::copy_n(result_.begin(), count, recvbuf);
    }
    barrier_.wait();
    return CollectiveResult::success;
}

} // namespace cpu_collectives::internal
