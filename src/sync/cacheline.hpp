#pragma once

#include <cstddef>

namespace cpu_collectives::sync {

/** Cache-line alignment helper for future shared counters and per-rank state. */
inline constexpr std::size_t kCacheLineSize = 64;

template <typename T>
struct alignas(kCacheLineSize) CachelinePadded {
    T value{};
};

} // namespace cpu_collectives::sync
