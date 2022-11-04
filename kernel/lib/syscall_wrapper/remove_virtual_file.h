#pragma once
#include "syscalls.h"

static inline uint64_t remove_virtual_file(const char *path) {
    return syscall(SYS_remove_virtual_file, (uint64_t)path);
}
