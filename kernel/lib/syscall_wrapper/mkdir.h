#pragma once
#include "syscalls.h"

static inline uint64_t mkdir(const char* path, mode_t mode) {
    return syscall(SYS_mkdir, path, mode);
}
