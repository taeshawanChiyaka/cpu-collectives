# cpu-collectives

A small, correctness-first CPU collective communication library. The initial prototype provides an in-process communicator for a fixed group of threads and a reusable barrier. The project is intended to explore high-performance CPU collectives, with Apple Silicon as an early optimization target and portable C++ as the baseline.

This is an experimental project, not a drop-in replacement for NCCL. No performance claims are made until reproducible benchmarks demonstrate them.

## Current scope

- C API with opaque communicator handles.
- Fixed-size, single-process thread groups.
- Reusable barrier, status reporting, lifecycle checks, tests, and a microbenchmark.
- C++20 implementation with no third-party dependencies.

Collectives such as all-reduce, broadcast, gather, and scatter are not implemented yet.

## Build

Requires CMake 3.20+ and a C++20-capable compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the example:

```sh
./build/examples/hello_barrier
```

Run the barrier benchmark (arguments: participant count, iterations):

```sh
./build/benchmarks/bench_barrier 4 100000
```

Enable AddressSanitizer and UndefinedBehaviorSanitizer on supported Clang/GCC toolchains with `-DCC_ENABLE_SANITIZERS=ON`. Sanitizers are for correctness checking, not performance measurement.

## Quick API sketch

Each participating thread uses a unique rank from `0` to `participant_count - 1`. All ranks must call barriers in the same order. Create the communicator before starting worker threads and destroy it only after every worker has finished using it.

```cpp
#include <cpu_collectives/cpu_collectives.h>

cc_communicator_t* comm = nullptr;
if (cc_communicator_create(4, &comm) == CC_STATUS_OK) {
    // Start four threads; each calls cc_barrier(comm, its_unique_rank).
    // Join all threads before destroying the communicator.
    cc_communicator_destroy(comm);
}
```

See [docs/api.md](docs/api.md) for API contracts, [docs/architecture.md](docs/architecture.md) for design boundaries, [docs/benchmarks.md](docs/benchmarks.md) for measurement guidance, and [docs/porting.md](docs/porting.md) for platform notes.
