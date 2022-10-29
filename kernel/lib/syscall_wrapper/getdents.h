#pragma once
#include "syscalls.h"

static inline uint64_t getdents(uint64_t fd, void* dirp, uint64_t count) {
    return syscall(SYS_getdents, fd, dirp, count);
}
