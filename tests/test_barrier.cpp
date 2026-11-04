#include "cpu_collectives/cpu_collectives.h"

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    constexpr std::size_t participants = 4;
    constexpr int rounds = 100;
    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(participants, &comm) != CC_STATUS_OK) {
        std::cerr << "communicator creation failed\n";
        return 1;
    }

    std::atomic<int> completed{0};
    std::vector<std::thread> workers;
    workers.reserve(participants);
    for (std::size_t rank = 0; rank < participants; ++rank) {
        workers.emplace_back([&, rank] {
            for (int i = 0; i < rounds; ++i) {
                if (cc_barrier(comm, rank) != CC_STATUS_OK) {
                    return;
                }
            }
            ++completed;
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    cc_communicator_destroy(comm);
    if (completed != static_cast<int>(participants)) {
        std::cerr << "not all workers completed all barrier rounds\n";
        return 1;
    }
    return 0;
}
