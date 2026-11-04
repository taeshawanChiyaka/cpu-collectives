# cpu-collectives

A small, correctness-first CPU collective communication library. The initial prototype provides an in-process communicator for a fixed group of threads, a reusable barrier, and a double-precision sum all-reduce. The project is intended to explore high-performance CPU collectives, with Apple Silicon as an early optimization target and portable C++ as the baseline.

This is an experimental project, not a drop-in replacement for NCCL. No performance claims are made until reproducible benchmarks demonstrate them.

## Current scope

- C API with opaque communicator handles.
- Fixed-size, single-process thread groups.
- Reusable barrier and `cc_all_reduce_sum_f64` correctness-first implementation.
- C++20 implementation with no third-party dependencies.

Other collectives, additional datatypes, multi-process communication, and architecture-specific kernels are not implemented yet.

## Build and test

Requires CMake 3.20+ and a C++20-capable compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the barrier example:

```sh
./build/examples/hello_barrier
```

## Benchmarks

Barrier (participants, iterations):

```sh
./build/benchmarks/bench_barrier 4 100000
```

All-reduce (participants, double elements, iterations, trials):

```sh
./build/benchmarks/bench_all_reduce 4 4096 1000 5
```

See [docs/benchmarks.md](docs/benchmarks.md) for the output interpretation and measurement guidance. Enable AddressSanitizer and UndefinedBehaviorSanitizer on supported Clang/GCC toolchains with `-DCC_ENABLE_SANITIZERS=ON`; sanitizers are for correctness checking, not performance measurement.

## Quick API sketch

Each participating thread uses a unique rank from `0` to `participant_count - 1`. All ranks must call collectives in the same order. Create the communicator before starting worker threads and destroy it only after every worker has finished using it.

```cpp
#include <cpu_collectives/cpu_collectives.h>

cc_communicator_t* comm = nullptr;
if (cc_communicator_create(2, &comm) == CC_STATUS_OK) {
    // Each of two worker threads calls all-reduce with its unique rank.
    double input = 1.0;
    double output = 0.0;
    // cc_all_reduce_sum_f64(comm, rank, &input, &output, 1);
    // Join both threads before destroying the communicator.
    cc_communicator_destroy(comm);
}
```

See [docs/api.md](docs/api.md) for API contracts, [docs/architecture.md](docs/architecture.md) for design boundaries, and [docs/porting.md](docs/porting.md) for platform notes.
