#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>

namespace cpu_collectives::sync {

/** Reusable generation-based barrier for a fixed number of participants. */
class Barrier {
public:
    explicit Barrier(std::size_t participant_count);
    void wait();

private:
    const std::size_t participant_count_;
    std::size_t arrived_ = 0;
    std::size_t generation_ = 0;
    std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace cpu_collectives::sync
