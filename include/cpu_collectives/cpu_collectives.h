#ifndef CPU_COLLECTIVES_CPU_COLLECTIVES_H
#define CPU_COLLECTIVES_CPU_COLLECTIVES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cc_communicator cc_communicator_t;

typedef enum cc_status {
    CC_STATUS_OK = 0,
    CC_STATUS_INVALID_ARGUMENT = 1,
    CC_STATUS_OUT_OF_MEMORY = 2,
    CC_STATUS_INTERNAL_ERROR = 3
} cc_status_t;

typedef enum cc_all_reduce_algorithm {
    /** Single-threaded reduction on rank 0; simple reference baseline. */
    CC_ALL_REDUCE_ALGORITHM_BASELINE = 0,
    /** Partition output elements across ranks so reduction work runs in parallel. */
    CC_ALL_REDUCE_ALGORITHM_PARALLEL = 1
} cc_all_reduce_algorithm_t;

/** Return a static description of a status code; do not free the result. */
const char* cc_status_string(cc_status_t status);

/** Create a fixed-size in-process communicator. */
cc_status_t cc_communicator_create(size_t participant_count,
                                   cc_communicator_t** out_comm);

/** Return the participant count, or zero if comm is null. */
size_t cc_communicator_size(const cc_communicator_t* comm);

/**
 * Wait until every participant reaches this barrier generation.
 * Each participant must use a unique in-range rank and call collectives in
 * the same order. Rank uniqueness is a caller responsibility in this version.
 */
cc_status_t cc_barrier(cc_communicator_t* comm, size_t rank);

/**
 * Sum `count` double values across all communicator participants and write
 * the complete sum to each participant's recvbuf. This selects the rank-0
 * baseline algorithm. Every rank must call operations in the same order.
 */
cc_status_t cc_all_reduce_sum_f64(cc_communicator_t* comm, size_t rank,
                                  const double* sendbuf, double* recvbuf,
                                  size_t count);

/**
 * All-reduce with an explicit algorithm for comparative benchmarking.
 * Baseline reduces all elements serially on rank 0. Parallel divides the
 * output range into contiguous chunks; each rank sums one chunk. The algorithms
 * have identical buffer, rank, count, synchronization, and overlap requirements
 * documented for cc_all_reduce_sum_f64. All ranks must select the same algorithm.
 */
cc_status_t cc_all_reduce_sum_f64_with_algorithm(
    cc_communicator_t* comm, size_t rank, const double* sendbuf, double* recvbuf,
    size_t count, cc_all_reduce_algorithm_t algorithm);

/** Destroy a communicator after all threads have stopped using it. Null is OK. */
void cc_communicator_destroy(cc_communicator_t* comm);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CPU_COLLECTIVES_CPU_COLLECTIVES_H */
