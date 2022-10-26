#pragma once
#include <types.h>
struct time{
    uint64_t tv_sec;
    uint64_t tv_nsec;
};
struct stat{
    uint32_t st_mode;
    struct time st_mtim;
    struct time st_ctim;
    struct time st_atim;
    uint64_t st_size;
    uint64_t st_ino;
    uint64_t st_nlink;
    uint64_t st_uid;
    uint64_t st_gid;
    uint64_t st_rdev;
    uint64_t st_blksize;
    uint64_t st_blocks;
};
