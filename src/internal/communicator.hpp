#pragma once

#include "../sync/barrier.hpp"

#include <cstddef>

namespace cpu_collectives::internal {

class Communicator {
public:
    explicit Communicator(std::size_t participant_count)
        : participant_count_(participant_count), barrier_(participant_count) {}

    [[nodiscard]] std::size_t size() const noexcept { return participant_count_; }
    void barrier() { barrier_.wait(); }

private:
    std::size_t participant_count_;
    sync::Barrier barrier_;
};

} // namespace cpu_collectives::internal
