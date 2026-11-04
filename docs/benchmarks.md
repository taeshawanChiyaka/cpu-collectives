# Benchmarking

The microbenchmarks are not a complete performance characterization. Build an optimized configuration and run the correctness tests before measuring:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Barrier

```sh
./build/benchmarks/bench_barrier 4 100000
```

The arguments are participant count and iterations.

## All-reduce

```sh
./build/benchmarks/bench_all_reduce 4 4096 1000 5
```

Arguments are `[participants] [elements] [iterations] [trials]`. Defaults are 2 participants, 1024 double elements, 1000 measured collectives, and 5 trials. Each worker performs up to 100 warmup collectives before timing; thread creation is excluded from the reported interval. The benchmark validates the final output of every rank after each trial and exits with an error if results are wrong.

Each trial reports elapsed time, average microseconds per all-reduce, and input MiB/s per rank. The throughput number is the per-rank input payload divided by collective latency; it is not aggregate system bandwidth and should not be compared directly with vendor-library bandwidth figures. Payload bytes per rank are `elements * sizeof(double)`.

For meaningful comparisons, record the exact CPU model, OS version, compiler and version, build type, participant count, payload size, iteration count, and whether the machine was under load. Run multiple trials and report the distribution (for example median and range), not only the best result. Avoid sanitizer builds for performance measurements. This initial all-reduce implementation is a simple rank-0 reduction baseline; results are for characterization, not a performance claim.
