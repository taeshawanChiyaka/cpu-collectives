#include "cpu_collectives/cpu_collectives.h"

#include <iostream>
#include <thread>
#include <vector>

int main() {
    constexpr std::size_t participants = 4;
    cc_communicator_t* comm = nullptr;
    const cc_status_t create_status = cc_communicator_create(participants, &comm);
    if (create_status != CC_STATUS_OK) {
        std::cerr << "could not create communicator: " << cc_status_string(create_status) << '\n';
        return 1;
    }

    std::vector<std::thread> workers;
    workers.reserve(participants);
    for (std::size_t rank = 0; rank < participants; ++rank) {
        workers.emplace_back([comm, rank] {
            std::cout << "rank " << rank << " waiting at barrier\n";
            const cc_status_t status = cc_barrier(comm, rank);
            if (status != CC_STATUS_OK) {
                std::cerr << "rank " << rank << " failed: " << cc_status_string(status) << '\n';
                return;
            }
            std::cout << "rank " << rank << " passed barrier\n";
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    cc_communicator_destroy(comm);
    return 0;
}
