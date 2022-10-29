#pragma once
#include <types.h>

#define __S_IFDIR 0040000  /* Directory.  */
#define __S_IFCHR 0020000  /* Character device.  */
#define __S_IFBLK 0060000  /* Block device.  */
#define __S_IFREG 0100000  /* Regular file.  */
#define __S_IFIFO 0010000  /* FIFO.  */
#define __S_IFLNK 0120000  /* Symbolic link.  */
#define __S_IFSOCK 0140000 /* Socket.  */

#define S_IFDIR __S_IFDIR
#define S_IFCHR __S_IFCHR
#define S_IFBLK __S_IFBLK
#define S_IFREG __S_IFREG
#define S_IFIFO __S_IFIFO
#define S_IFLNK __S_IFLNK
#define S_IFSOCK __S_IFSOCK

struct time{
    uint64_t tv_sec;
    uint64_t tv_nsec;
};
struct stat{
    mode_t st_mode;
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
