# Contributing

Thanks for your interest in cpu-collectives! The project is an experimental, correctness-first CPU collective library. Contributions that improve correctness, portability, documentation, and reproducible measurement are especially welcome.

## Before you start

- Check existing issues and pull requests for related work.
- For a substantial change, open an issue first to discuss the API or design.
- Keep changes focused. Avoid combining a new collective, an algorithm change, and unrelated cleanup in one pull request.
- Do not include benchmark claims without reproducible results and hardware/build details.

## Development setup

Requirements: CMake 3.20 or newer and a C++20-capable compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For supported Clang/GCC toolchains, run sanitizers where available:

```sh
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCC_ENABLE_SANITIZERS=ON
cmake --build build-asan --parallel
ctest --test-dir build-asan --output-on-failure
```

## Pull requests

A good pull request should:

1. Explain the problem and intended behavior.
2. Add or update tests for behavior changes, including error paths and edge cases.
3. Keep the public C API and its documentation synchronized.
4. Pass the CMake build and `ctest` locally; include the command and relevant results in the PR description.
5. For performance work, include before/after benchmark output, machine/CPU, OS, compiler, build type, parameters, and measurement method. Don’t use Debug or sanitizer results as performance results.
6. State known limitations and any API or ABI compatibility impact.

## Coding guidelines

- Prefer portable C++20 and keep platform-specific code isolated.
- Treat the scalar/reference path as the correctness baseline.
- Avoid undefined behavior, data races, and unsupported assumptions about CPU topology.
- Don’t make an algorithm the default solely because it wins on one payload or machine.
- Document synchronization and buffer ownership requirements.

## Conduct

Be respectful and constructive. Contributions are subject to the project’s license.
