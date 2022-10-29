#pragma once
#include "syscalls.h"

static inline uint64_t close(uint64_t fd) {
    return syscall(SYS_close, fd);
}
