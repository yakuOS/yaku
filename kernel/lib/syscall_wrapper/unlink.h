#pragma once
#include "syscalls.h"

static inline uint64_t unlink(const char* path) {
    return syscall(SYS_unlink, path);
}
