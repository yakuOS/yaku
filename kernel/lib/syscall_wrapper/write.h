#pragma once
#include "syscalls.h"

static inline uint64_t write(uint64_t fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}
