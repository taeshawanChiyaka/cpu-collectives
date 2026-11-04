#include "cpu_collectives/cpu_collectives.h"

#include <iostream>

int main() {
    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(0, &comm) != CC_STATUS_INVALID_ARGUMENT || comm != nullptr) {
        std::cerr << "zero participants should be rejected\n";
        return 1;
    }
    if (cc_communicator_create(1, nullptr) != CC_STATUS_INVALID_ARGUMENT) {
        std::cerr << "null output should be rejected\n";
        return 1;
    }
    if (cc_communicator_create(3, &comm) != CC_STATUS_OK || comm == nullptr) {
        std::cerr << "communicator creation failed\n";
        return 1;
    }
    if (cc_communicator_size(comm) != 3) {
        std::cerr << "unexpected communicator size\n";
        return 1;
    }
    if (cc_communicator_size(nullptr) != 0 ||
        cc_barrier(nullptr, 0) != CC_STATUS_INVALID_ARGUMENT ||
        cc_barrier(comm, 3) != CC_STATUS_INVALID_ARGUMENT) {
        std::cerr << "null or out-of-range argument handling failed\n";
        cc_communicator_destroy(comm);
        return 1;
    }
    cc_communicator_destroy(comm);
    cc_communicator_destroy(nullptr);
    return 0;
}
