#pragma once
#include "syscalls.h"

static inline uint64_t read(uint64_t fd, void* buf, size_t count) {
    return syscall(SYS_read, fd, buf, count);
}
