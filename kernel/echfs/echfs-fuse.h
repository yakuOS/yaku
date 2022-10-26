#pragma once

#define EPERM 1    /* Operation not permitted */
#define ENOENT 2   /* No such file or directory */
#define ESRCH 3    /* No such process */
#define EINTR 4    /* Interrupted system call */
#define EIO 5      /* I/O error */
#define ENXIO 6    /* No such device or address */
#define E2BIG 7    /* Argument list too long */
#define ENOEXEC 8  /* Exec format error */
#define EBADF 9    /* Bad file number */
#define ECHILD 10  /* No child processes */
#define EAGAIN 11  /* Try again */
#define ENOMEM 12  /* Out of memory */
#define EACCES 13  /* Permission denied */
#define EFAULT 14  /* Bad address */
#define ENOTBLK 15 /* Block device required */
#define EBUSY 16   /* Device or resource busy */
#define EEXIST 17  /* File exists */
#define EXDEV 18   /* Cross-device link */
#define ENODEV 19  /* No such device */
#define ENOTDIR 20 /* Not a directory */
#define EISDIR 21  /* Is a directory */
#define EINVAL 22  /* Invalid argument */
#define ENFILE 23  /* File table overflow */
#define EMFILE 24  /* Too many open files */
#define ENOTTY 25  /* Not a typewriter */
#define ETXTBSY 26 /* Text file busy */
#define EFBIG 27   /* File too large */
#define ENOSPC 28  /* No space left on device */
#define ESPIPE 29  /* Illegal seek */
#define EROFS 30   /* Read-only file system */
#define EMLINK 31  /* Too many links */
#define EPIPE 32   /* Broken pipe */
#define EDOM 33    /* Math argument out of domain of func */
#define ERANGE 34  /* Math result not representable */

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

#define __S_ISUID 04000 /* Set user ID on execution.  */
#define __S_ISGID 02000 /* Set group ID on execution.  */
#define __S_ISVTX 01000 /* Save swapped text after use (sticky).  */
#define __S_IREAD 0400  /* Read by owner.  */
#define __S_IWRITE 0200 /* Write by owner.  */
#define __S_IEXEC 0100  /* Execute by owner.  */

#define S_IRWXU (__S_IREAD | __S_IWRITE | __S_IEXEC)
#define S_IRWXG (S_IRWXU >> 3)
#define S_IRWXO (S_IRWXG >> 3)
typedef unsigned short mode_t;

#define ENOTEMPTY 39       /* Directory not empty */
#define ELOOP 40           /* Too many symbolic links encountered */
#define EWOULDBLOCK EAGAIN /* Operation would block */
#define ENOMSG 42          /* No message of desired type */
#define EIDRM 43           /* Identifier removed */
#define ECHRNG 44          /* Channel number out of range */
#define EL2NSYNC 45        /* Level 2 not synchronized */
#define EL3HLT 46          /* Level 3 halted */
#define EL3RST 47          /* Level 3 reset */
#define ELNRNG 48          /* Link number out of range */
#define EUNATCH 49         /* Protocol driver not attached */
#define ENOCSI 50          /* No CSI structure available */
#define EL2HLT 51          /* Level 2 halted */
#define EBADE 52           /* Invalid exchange */
#define EBADR 53           /* Invalid request descriptor */
#define EXFULL 54          /* Exchange full */
#define ENOANO 55          /* No anode */
#define EBADRQC 56         /* Invalid request code */
#define EBADSLT 57         /* Invalid slot */

#define FUSE_OPT_KEY_NONOPT -2

#define FUSE_OPT_END                                                                     \
    { NULL, 0, 0 }

#ifdef NDEBUG

#  define assert(condition) ((void)0)
#else
#  define assert(condition) /*implementation defined*/
#endif

static void* echfs_init(struct fuse_conn_info* conn);
static void echfs_destroy(
    void* data);
static int echfs_open(const char* file_path,
                                      struct fuse_file_info* file_info);
static int echfs_opendir(const char* dir_path, struct fuse_file_info* file_info);

static int echfs_fgetattr(const char *path, struct stat *stat,
        struct fuse_file_info *file_info);
void echfs_get_fuse_operations(struct fuse_operations *buffer);
int echfs_fuse_main(int argc, char* argv[]);
