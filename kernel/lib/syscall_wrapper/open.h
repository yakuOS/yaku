#pragma once
#include "syscalls.h"

static inline uint64_t open(const char* path, int flags) {
    return syscall(SYS_open, path, flags);
}
