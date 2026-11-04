#include "cpu_collectives/cpu_collectives.h"

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    const int participants = argc > 1 ? std::atoi(argv[1]) : 8;
    const int rounds = argc > 2 ? std::atoi(argv[2]) : 100000;
    if (participants <= 0 || rounds <= 0) {
        std::cerr << "participant count and rounds must be positive\n";
        return 2;
    }

    cc_communicator_t* comm = nullptr;
    if (cc_communicator_create(static_cast<std::size_t>(participants), &comm) != CC_STATUS_OK) {
        std::cerr << "communicator creation failed\n";
        return 1;
    }

    std::atomic<int> failures{0};
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(participants));
    for (int rank = 0; rank < participants; ++rank) {
        workers.emplace_back([&, rank] {
            for (int i = 0; i < rounds; ++i) {
                if (cc_barrier(comm, static_cast<std::size_t>(rank)) != CC_STATUS_OK) {
                    ++failures;
                    return;
                }
            }
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    cc_communicator_destroy(comm);

    if (failures != 0) {
        std::cerr << "barrier stress test reported failures\n";
        return 1;
    }
    return 0;
}
