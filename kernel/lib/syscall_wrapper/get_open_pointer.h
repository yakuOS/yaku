#pragma once
#include <types.h>
#include "syscalls.h"

static inline uint64_t get_open_pointer() {
    return syscall(SYS_get_open_pointer);
}
