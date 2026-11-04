#include "cpu_collectives/cpu_collectives.h"

#include <iostream>

int main() {
    if (cc_status_string(CC_STATUS_OK) == nullptr ||
        std::string(cc_status_string(CC_STATUS_OK)) != "success" ||
        std::string(cc_status_string(static_cast<cc_status_t>(99))) != "unknown status") {
        std::cerr << "status string test failed\n";
        return 1;
    }
    return 0;
}
