# Benchmarking

`bench_barrier` is a small microbenchmark, not a complete performance characterization. It reports elapsed time and average time per barrier round for a fixed number of participating threads.

Build in Release mode and run, for example:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/benchmarks/bench_barrier 4 100000
```

Arguments are optional: participant count defaults to 2 and iteration count defaults to 10000. Both must be positive integers. The reported average is wall-clock elapsed time divided by the iteration count; it includes thread scheduling and wake-up costs.

For meaningful comparisons, record the exact CPU model, OS version, compiler and version, CMake build type, participant count, iteration count, and whether the machine was under load. Run multiple trials, report distributions rather than only the best result, and compare against a simple baseline. Do not compare sanitizer builds to optimized builds. Avoid pinning workers or changing power settings unless those conditions are documented and reproducible.

Future benchmarks should cover collectives and payload sizes, separating setup from steady-state work. No benchmark result should be generalized across hardware without evidence.
