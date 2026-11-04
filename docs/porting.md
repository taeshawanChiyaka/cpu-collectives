# Porting and platform notes

The implementation uses standard C++20 synchronization primitives and is designed to build on supported C++20 toolchains. The initial goal is to keep platform-specific code out of the public API.

## Apple Silicon

Build natively on an Apple Silicon Mac with a recent Xcode Command Line Tools installation and CMake 3.20 or newer. The first implementation does not require Accelerate or architecture-specific intrinsics. Apple Silicon-specific work should follow profiling and remain behind a portable fallback. NEON may be useful for future data-reduction kernels, but it does not accelerate the current barrier by itself.

## Other CPUs

Use the same CMake build and test commands on other supported platforms. Performance can differ because of scheduler behavior, cache topology, and synchronization implementations. Treat benchmark output as hardware- and configuration-specific.

## Future backend guidance

Keep architecture-specific kernels isolated from the API and reference implementation. Add compile-time or runtime dispatch only when there is a measured need, and test optimized kernels against reference results. Document compiler flags and supported architectures for every optional backend.
