# Test results

Local validation of cpu-collectives.

- Date: 2026-11-04
- Revision: e28f9a1
- Machine identifier: MacBookAir10,1
- macOS version: 27.2
- Compiler: Apple clang version 21.0.0 (clang-2100.3.34.2)
- Build configuration: Debug
- Sanitizers: Disabled; the previous sanitizer configuration failed during CMake's compiler-flag check.
- Build: Passed
- Test command: ctest --test-dir build-debug --output-on-failure
- Result: 5 of 5 tests passed; total test time 2.44 seconds.

## Test cases

- status: passed
- lifecycle: passed
- barrier: passed
- all_reduce: passed
- stress_barrier: passed (2.42 seconds)
