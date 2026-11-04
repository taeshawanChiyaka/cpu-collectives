#pragma once

#include <chrono>
#include <cstddef>
#include <iostream>

namespace cc_bench {
using Clock = std::chrono::steady_clock;

inline void report(const char* name, std::size_t participants, std::size_t iterations,
                   Clock::duration elapsed) {
    const auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    const double average_ns = static_cast<double>(nanoseconds) / static_cast<double>(iterations);
    std::cout << name << ": participants=" << participants
              << " iterations=" << iterations
              << " elapsed_ms=" << std::chrono::duration<double, std::milli>(elapsed).count()
              << " average_ns_per_round=" << average_ns << '\n';
}
} // namespace cc_bench
