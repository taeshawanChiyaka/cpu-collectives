#include "cpu_collectives/cpu_collectives.h"

extern "C" const char* cc_status_string(cc_status_t status) {
    switch (status) {
    case CC_STATUS_OK:
        return "success";
    case CC_STATUS_INVALID_ARGUMENT:
        return "invalid argument";
    case CC_STATUS_OUT_OF_MEMORY:
        return "out of memory";
    case CC_STATUS_INTERNAL_ERROR:
        return "internal error";
    default:
        return "unknown status";
    }
}
