# Benchmarking

Build an optimized configuration and run the correctness tests before measuring:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Compare all-reduce algorithms

```sh
./build/benchmarks/bench_all_reduce 4 4096 1000 5
```

Arguments are `[participants] [elements] [iterations] [trials]`. Defaults are 2 participants, 1024 double elements, 1000 measured collectives, and 5 trials. Both algorithms are measured in each trial: `baseline` reduces every element serially on rank 0; `parallel` divides elements into balanced contiguous chunks so each rank sums a separate range. Each worker performs up to 100 warmup collectives; thread creation is excluded from timed intervals. Outputs are validated after every trial.

Each result reports elapsed time, average microseconds per collective, and input MiB/s per rank. Throughput means per-rank input payload divided by collective latency; it is not aggregate bandwidth and should not be compared directly with vendor-library bandwidth figures. The variants use the same staged input storage, synchronization, and result-copy path, so this is an initial algorithm comparison rather than a fully tuned benchmark.

A small payload may favor the baseline because the parallel variant adds work distribution without enough reduction work to offset it. The parallel implementation is a candidate for larger payloads, not an assumption that it is faster on every CPU. Record CPU model, OS, compiler/version, build type, participant count, payload size, and load. Run multiple trials and compare medians/ranges; do not infer performance from one run. Avoid sanitizer builds for timing.

## Barrier

```sh
./build/benchmarks/bench_barrier 4 100000
```

The arguments are participant count and iterations.
