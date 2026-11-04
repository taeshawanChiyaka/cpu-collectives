# Architecture

## Current design

The first milestone is deliberately limited to synchronization among a fixed number of threads in one process. A public C ABI sits over a small C++20 implementation:

1. `include/cpu_collectives/cpu_collectives.h` defines opaque handles, status values, and stable C-callable functions.
2. `src/communicator.cpp` validates API arguments and owns the internal communicator.
3. `src/internal/communicator.hpp` holds implementation details, including the participant count, reusable barrier, and reduction state.
4. `src/internal/communicator.cpp` implements the correctness-first all-reduce and the comparison baseline/parallel variants.
5. `src/sync/barrier.cpp` implements a reusable generation-based condition-variable barrier.
6. Tests verify status values, lifecycle boundaries, barrier behavior, and collective correctness. The benchmark measures synchronization and reduction cost separately from correctness tests.

## Thread and rank contract

A communicator has a fixed participant count. Each participating thread must use one unique rank in `[0, participant_count)`, and all ranks must invoke collective operations in the same order. The implementation does not currently enforce unique rank usage; callers are responsible for that contract.

The caller must ensure all worker threads have stopped using a communicator before destroying it. Concurrent destruction and API calls are unsupported. A thread that exits or fails to arrive while a collective is in progress can cause peers to block.

## Planned layers

- API and semantics: define collective ordering, datatype support, errors, buffer ownership, and thread safety before adding more operations.
- Algorithm layer: start with simple reference implementations, then select algorithms based on measured participant counts and message sizes.
- CPU backend: retain a portable scalar/reference implementation and isolate architecture-specific kernels behind dispatch boundaries. Apple Silicon optimization should come after a verified baseline on a broader hardware set.
- Runtime: keep the initial in-process communicator distinct from any future shared-memory multi-process transport.

## Open design questions

- Should participant identity be bound to a rank-specific endpoint rather than passed on each operation?
- What error model should handle missing participants and mismatched collective order?
- Which numeric types and reduction operations are required for an initial all-reduce?
- Which benchmarks and competitor baselines best represent intended use cases?
- What, if any, affinity or topology controls are portable and worthwhile?

Do not infer that Apple Silicon's unified memory eliminates cache-coherence, false-sharing, or synchronization costs. Validate each design choice on actual hardware.
