# API reference

The public API is declared in `include/cpu_collectives/cpu_collectives.h` and is callable from C or C++.

## Status codes

- `CC_STATUS_OK`: operation succeeded.
- `CC_STATUS_INVALID_ARGUMENT`: a required pointer is null or a value is outside the documented range.
- `CC_STATUS_OUT_OF_MEMORY`: communicator allocation failed.
- `CC_STATUS_INTERNAL_ERROR`: an unexpected implementation error occurred.

`cc_status_string` returns a static, null-terminated description for known status values and a fallback description for unknown values. The returned pointer must not be freed.

## Communicator lifecycle

`cc_communicator_create(participant_count, &comm)` creates a communicator for a fixed group. The count must be greater than zero, and `out_comm` must be non-null. On failure, a valid output pointer is set to null before returning.

`cc_communicator_size(comm)` returns the participant count, or zero for a null handle.

`cc_communicator_destroy(comm)` releases the handle. Passing null is a no-op. Destroy only after all participating threads have stopped calling into the communicator. Destruction concurrent with an operation is unsupported.

## Barrier

`cc_barrier(comm, rank)` waits until all participants have arrived at that barrier generation. `rank` must be in `[0, cc_communicator_size(comm))`. Every participant must call barriers in the same order. This implementation does not detect duplicate ranks, mismatched call order, or participants that never arrive.

The rank argument is currently used for range validation only; it is not an endpoint or a guarantee of rank uniqueness. Invalid arguments return immediately without entering the barrier.

## Example

```cpp
cc_communicator_t* comm = nullptr;
if (cc_communicator_create(2, &comm) != CC_STATUS_OK) {
    return 1;
}
// Have exactly two worker threads call cc_barrier(comm, 0) and
// cc_barrier(comm, 1), respectively, then join both threads.
cc_communicator_destroy(comm);
```
