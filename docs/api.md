# API reference

The public API is declared in `include/cpu_collectives/cpu_collectives.h` and is callable from C or C++.

## Status codes

- `CC_STATUS_OK`: operation succeeded.
- `CC_STATUS_INVALID_ARGUMENT`: a required pointer is null or a value is outside the documented range.
- `CC_STATUS_OUT_OF_MEMORY`: communicator allocation failed.
- `CC_STATUS_INTERNAL_ERROR`: an unexpected implementation error occurred.

`cc_status_string` returns a static, null-terminated description for known status values and a fallback description for unknown values. The returned pointer must not be freed.

## Communicator lifecycle

`cc_communicator_create(participant_count, &comm)` creates a communicator for a fixed group. The count must be greater than zero, and `out_comm` must be non-null. On failure, a valid output pointer is not produced.

`cc_communicator_size(comm)` returns the participant count, or zero for a null handle.

`cc_communicator_destroy(comm)` releases the handle. Passing null is a no-op. Destroy only after all participating threads have stopped calling into the communicator.

## Barrier

`cc_barrier(comm, rank)` waits until all participants have arrived at that barrier generation. `rank` must be in `[0, cc_communicator_size(comm))`. Every participant must call barriers in the same order. The rank argument is currently used for range validation only; it is not an endpoint or a guarantee of rank uniqueness.

## All-reduce

`cc_all_reduce_sum_f64(comm, rank, sendbuf, recvbuf, count)` sums `count` double values from every rank and writes the full reduction result to each rank's `recvbuf`. `rank` must be in `[0, participant_count)`, and `count` may be zero. When `count != 0`, both `sendbuf` and `recvbuf` must be non-null. All ranks must invoke the collective in the same order.

`cc_all_reduce_sum_f64_with_algorithm(comm, rank, sendbuf, recvbuf, count, algorithm)` selects an explicit execution strategy for benchmarking:

- `CC_ALL_REDUCE_ALGORITHM_BASELINE`: serial reduction on rank 0
- `CC_ALL_REDUCE_ALGORITHM_PARALLEL`: partition output elements across ranks so each rank reduces its own chunk

The same validation and synchronization rules apply as for `cc_all_reduce_sum_f64`. All ranks must pick the same algorithm for a given collective call.

## Example

```cpp
cc_communicator_t* comm = nullptr;
if (cc_communicator_create(2, &comm) != CC_STATUS_OK) {
    return 1;
}

// Have two worker threads call the collective with unique ranks.
double input = 1.0;
double output = 0.0;
// cc_all_reduce_sum_f64(comm, 0, &input, &output, 1);
// cc_all_reduce_sum_f64(comm, 1, &input, &output, 1);

cc_communicator_destroy(comm);
```
