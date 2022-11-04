#pragma once
#include "syscalls.h"
#include <lib/fuse.h>

static inline uint64_t mknod(const char* path, mode_t mode, dev_t dev) {
    return syscall(SYS_mknod, path, mode, dev);
}
