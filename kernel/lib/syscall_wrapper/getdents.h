#pragma once
#include <types.h>
#include "syscalls.h"

struct dirent {
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name;
    mode_t type;
};

static inline uint64_t getdents(uint64_t fd, void* dirp, uint64_t count) {
    return syscall(SYS_getdents, fd, dirp, count);
}
