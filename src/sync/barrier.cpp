#include "barrier.hpp"

#include <stdexcept>

namespace cpu_collectives::sync {

Barrier::Barrier(std::size_t participant_count)
    : participant_count_(participant_count) {
    if (participant_count == 0) {
        throw std::invalid_argument("barrier participant count must be positive");
    }
}

void Barrier::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    const std::size_t current_generation = generation_;
    ++arrived_;

    if (arrived_ == participant_count_) {
        arrived_ = 0;
        ++generation_;
        lock.unlock();
        condition_.notify_all();
        return;
    }

    condition_.wait(lock, [this, current_generation] {
        return generation_ != current_generation;
    });
}

} // namespace cpu_collectives::sync
