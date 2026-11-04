# Architecture

## Current design

The first milestone is deliberately limited to synchronization among a fixed number of threads in one process. A public C ABI sits over a small C++20 implementation:

1. `include/cpu_collectives/cpu_collectives.h` defines opaque handles, status values, and stable C-callable functions.
2. `src/communicator.cpp` validates API arguments and owns the internal communicator.
3. `src/internal/communicator.hpp` holds implementation details, including the participant count and reusable barrier.
4. `src/sync/barrier.cpp` implements a generation-based condition-variable barrier. It is a correctness-first baseline, not a claimed high-performance design.
5. Tests verify status values, lifecycle errors, barrier behavior, and repeated use. The benchmark measures synchronization cost separately from correctness tests.

## Thread and rank contract

A communicator has a fixed participant count. Each participating thread must use one unique rank in `[0, participant_count)`, and all ranks must invoke collective operations in the same order. The current barrier synchronizes arrivals but does not track rank uniqueness across calls. Calling with duplicate ranks is a caller error and can cause the other participants to wait indefinitely.

The caller must ensure all worker threads have stopped using a communicator before destroying it. Concurrent destruction and API calls are unsupported. A thread that exits or fails to arrive while others are waiting can leave them blocked; timeout and cancellation support are future design questions.

## Planned layers

- **API and semantics:** define collective ordering, datatype support, errors, buffer ownership, and thread-safety before adding data operations.
- **Algorithm layer:** add simple reference implementations first, then choose among algorithms based on measured participant counts and message sizes.
- **CPU backend:** retain a portable scalar/reference implementation and isolate architecture-specific kernels behind dispatch boundaries. Investigate ARM NEON on Apple Silicon only after profiling.
- **Runtime:** keep the initial in-process communicator distinct from any future shared-memory multi-process transport.

## Open design questions

- Should participant identity be bound to a rank-specific endpoint rather than passed on each operation?
- What error model should handle missing participants and mismatched collective order?
- Which numeric types and reduction operations are required for an initial all-reduce?
- Which benchmarks and competitor baselines best represent intended use cases?
- What, if any, affinity or topology controls are portable and worthwhile?

Do not infer that Apple Silicon's unified memory eliminates cache-coherence, false-sharing, or synchronization costs. Validate each design choice on actual hardware.
