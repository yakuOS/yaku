#pragma once
#include "syscalls.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

static inline uint64_t lseek(uint64_t fd, uint64_t offset, int whence) {
    return syscall(SYS_lseek, fd, offset, whence);
}
