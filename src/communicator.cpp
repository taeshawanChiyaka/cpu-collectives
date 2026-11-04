#include "cpu_collectives/cpu_collectives.h"

#include "internal/communicator.hpp"

#include <new>

struct cc_communicator {
    explicit cc_communicator(size_t participant_count)
        : implementation(participant_count) {}

    cpu_collectives::internal::Communicator implementation;
};

extern "C" cc_status_t cc_communicator_create(size_t participant_count,
                                              cc_communicator_t** out_comm) {
    if (out_comm == nullptr) {
        return CC_STATUS_INVALID_ARGUMENT;
    }
    *out_comm = nullptr;
    if (participant_count == 0) {
        return CC_STATUS_INVALID_ARGUMENT;
    }

    try {
        *out_comm = new cc_communicator(participant_count);
        return CC_STATUS_OK;
    } catch (const std::bad_alloc&) {
        return CC_STATUS_OUT_OF_MEMORY;
    } catch (...) {
        return CC_STATUS_INTERNAL_ERROR;
    }
}

extern "C" size_t cc_communicator_size(const cc_communicator_t* comm) {
    return comm == nullptr ? 0 : comm->implementation.size();
}

extern "C" cc_status_t cc_barrier(cc_communicator_t* comm, size_t rank) {
    if (comm == nullptr || rank >= comm->implementation.size()) {
        return CC_STATUS_INVALID_ARGUMENT;
    }
    try {
        comm->implementation.barrier();
        return CC_STATUS_OK;
    } catch (...) {
        return CC_STATUS_INTERNAL_ERROR;
    }
}

namespace {
cc_status_t invoke_all_reduce(cc_communicator_t* comm, size_t rank,
                              const double* sendbuf, double* recvbuf,
                              size_t count,
                              cpu_collectives::internal::AllReduceAlgorithm algorithm) {
    if (comm == nullptr || rank >= comm->implementation.size()) {
        return CC_STATUS_INVALID_ARGUMENT;
    }
    try {
        switch (comm->implementation.all_reduce_sum_f64(
            rank, sendbuf, recvbuf, count, algorithm)) {
        case cpu_collectives::internal::CollectiveResult::success:
            return CC_STATUS_OK;
        case cpu_collectives::internal::CollectiveResult::invalid_argument:
            return CC_STATUS_INVALID_ARGUMENT;
        case cpu_collectives::internal::CollectiveResult::out_of_memory:
            return CC_STATUS_OUT_OF_MEMORY;
        }
    } catch (...) {
        return CC_STATUS_INTERNAL_ERROR;
    }
    return CC_STATUS_INTERNAL_ERROR;
}
} // namespace

extern "C" cc_status_t cc_all_reduce_sum_f64(cc_communicator_t* comm, size_t rank,
                                               const double* sendbuf, double* recvbuf,
                                               size_t count) {
    return invoke_all_reduce(comm, rank, sendbuf, recvbuf, count,
                             cpu_collectives::internal::AllReduceAlgorithm::baseline);
}

extern "C" cc_status_t cc_all_reduce_sum_f64_with_algorithm(
    cc_communicator_t* comm, size_t rank, const double* sendbuf, double* recvbuf,
    size_t count, cc_all_reduce_algorithm_t algorithm) {
    using cpu_collectives::internal::AllReduceAlgorithm;
    if (algorithm != CC_ALL_REDUCE_ALGORITHM_BASELINE &&
        algorithm != CC_ALL_REDUCE_ALGORITHM_PARALLEL) {
        return CC_STATUS_INVALID_ARGUMENT;
    }
    const auto internal_algorithm = algorithm == CC_ALL_REDUCE_ALGORITHM_PARALLEL
                                        ? AllReduceAlgorithm::parallel
                                        : AllReduceAlgorithm::baseline;
    return invoke_all_reduce(comm, rank, sendbuf, recvbuf, count, internal_algorithm);
}

extern "C" void cc_communicator_destroy(cc_communicator_t* comm) {
    delete comm;
}
