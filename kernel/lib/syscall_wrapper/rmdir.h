#pragma once
#include "syscalls.h"

static inline uint64_t rmdir(const char* path) {
    return syscall(SYS_rmdir, path);
}
