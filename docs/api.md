# API reference

The public API is declared in `include/cpu_collectives/cpu_collectives.h` and is callable from C or C++.

## Status codes

- `CC_STATUS_OK`: operation succeeded.
- `CC_STATUS_INVALID_ARGUMENT`: a required pointer is null or a value is outside the documented range.
- `CC_STATUS_OUT_OF_MEMORY`: communicator or collective scratch allocation failed.
- `CC_STATUS_INTERNAL_ERROR`: an unexpected implementation error occurred.

`cc_status_string` returns a static, null-terminated description for known status values and a fallback description for unknown values. The returned pointer must not be freed.

## Communicator lifecycle

`cc_communicator_create(participant_count, &comm)` creates a communicator for a fixed group. The count must be greater than zero, and `out_comm` must be non-null. On failure, a valid output pointer is set to null before returning.

`cc_communicator_size(comm)` returns the participant count, or zero for a null handle.

`cc_communicator_destroy(comm)` releases the handle. Passing null is a no-op. Destroy only after all participating threads have stopped calling into the communicator. Destruction concurrent with an operation is unsupported.

## Barrier

`cc_barrier(comm, rank)` waits until all participants have arrived at that barrier generation. `rank` must be in `[0, cc_communicator_size(comm))`. Every rank must call barriers in the same order. The implementation does not detect duplicate ranks or participants that never arrive.

## All-reduce sum for double

`cc_all_reduce_sum_f64(comm, rank, sendbuf, recvbuf, count)` sums `count` double values across all participants and writes the result to every participant's receive buffer. Every participant must call collectives in the same order with the same count and a unique rank. For nonzero count, both buffers must be non-null. Zero count is allowed and buffers may be null. Input and output may be identical or overlap because input is staged before output is written.

If all ranks enter with valid communicator/ranks but any buffer is null for a nonzero count, or the ranks disagree on count, all ranks return `CC_STATUS_INVALID_ARGUMENT`. If scratch allocation fails, all ranks return `CC_STATUS_OUT_OF_MEMORY`. This is a simple rank-0 reduction baseline with per-rank staging and synchronization; it is intended for correctness, not a performance claim. A participant that does not enter the collective, or duplicate ranks, can leave peers waiting.

## Example

```cpp
cc_communicator_t* comm = nullptr;
if (cc_communicator_create(2, &comm) != CC_STATUS_OK) {
    return 1;
}

// Run once on each of two worker threads, using its unique rank.
double send[2] = {1.0, 2.0};
double recv[2] = {};
cc_status_t status = cc_all_reduce_sum_f64(comm, rank, send, recv, 2);

// Join both workers before destroying the communicator.
cc_communicator_destroy(comm);
```
