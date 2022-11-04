#pragma once
#include <types.h>
struct fuse;
typedef unsigned short mode_t;
# define __STD_TYPE		typedef
# define __UQUAD_TYPE unsigned long int
#define __DEV_T_TYPE __UQUAD_TYPE
__STD_TYPE __DEV_T_TYPE __dev_t;	/* Type of device numbers.  */
typedef __dev_t dev_t;
typedef int uid_t;
typedef int gid_t;
# define __SQUAD_TYPE		long int
#define __OFF64_T_TYPE		__SQUAD_TYPE
__STD_TYPE __OFF64_T_TYPE __off64_t;
// typedef __off64_t off_t;
# define __SWORD_TYPE		long int
#define __SSIZE_T_TYPE		__SWORD_TYPE
__STD_TYPE __SSIZE_T_TYPE __ssize_t; /* Type of a byte count, or error.  */
typedef __ssize_t ssize_t;

// struct fuse_chan {
// 	struct fuse_chan_ops op;

// 	struct fuse_session *se;

// 	int fd;
// 	size_t bufsize;

// 	void *data;

// 	int compat;
// };
enum fuse_fill_dir_flags {
	/**
	 * "Plus" mode: all file attributes are valid
	 *
	 * The attributes are used by the kernel to prefill the inode cache
	 * during a readdir.
	 *
	 * It is okay to set FUSE_FILL_DIR_PLUS if FUSE_READDIR_PLUS is not set
	 * and vice versa.
	 */
	FUSE_FILL_DIR_PLUS = (1 << 1)
};
typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
				const struct stat *stbuf, off_t off,
				enum fuse_fill_dir_flags flags);


struct fuse_file_info {
	/** Open flags.	 Available in open() and release() */
	int flags;

	/** Old file handle, don't use */
	unsigned long fh_old;

	/** In case of a write operation indicates if this was caused by a
	    writepage */
	int writepage;

	/** Can be filled in by open, to use direct I/O on this file.
	    Introduced in version 2.4 */
	unsigned int direct_io : 1;

	/** Can be filled in by open, to indicate, that cached file data
	    need not be invalidated.  Introduced in version 2.4 */
	unsigned int keep_cache : 1;

	/** Indicates a flush operation.  Set in flush operation, also
	    maybe set in highlevel lock operation and lowlevel release
	    operation.	Introduced in version 2.6 */
	unsigned int flush : 1;

	/** Can be filled in by open, to indicate that the file is not
	    seekable.  Introduced in version 2.8 */
	unsigned int nonseekable : 1;

	/* Indicates that flock locks for this file should be
	   released.  If set, lock_owner shall contain a valid value.
	   May only be set in ->release().  Introduced in version
	   2.9 */
	unsigned int flock_release : 1;

	/** Padding.  Do not use*/
	unsigned int padding : 27;

	/** File handle.  May be filled in by filesystem in open().
	    Available in all other file operations */
	uint64_t fh;

	/** Lock owner id.  Available in locking operations and flush */
	uint64_t lock_owner;
};
struct fuse_opt {
	/** Matching template and optional parameter formatting */
	const char *templ;

	/**
	 * Offset of variable within 'data' parameter of fuse_opt_parse()
	 * or -1
	 */
	unsigned long offset;

	/**
	 * Value to set the variable to, or to be passed as 'key' to the
	 * processing function.	 Ignored if template has a format
	 */
	int value;
};
// typedef struct fuse_dirhandle *fuse_dirh_t;
// typedef int (*fuse_dirfil_t) (fuse_dirh_t h, const char *name, int type,
// 			      ino_t ino);
struct timespec {
	uint64_t		tv_sec;		/* seconds */
	uint64_t		tv_nsec;	/* nanoseconds */
};
struct fuse_operations {
	/** Get file attributes.
	 *
	 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
	 * ignored.	 The 'st_ino' field is ignored except if the 'use_ino'
	 * mount option is given.
	 */
	int (*getattr) (const char *, struct stat *);

	/** Read the target of a symbolic link
	 *
	 * The buffer should be filled with a null terminated string.  The
	 * buffer size argument includes the space for the terminating
	 * null character.	If the linkname is too long to fit in the
	 * buffer, it should be truncated.	The return value should be 0
	 * for success.
	 */
	int (*readlink) (const char *, char *, size_t);

	/* Deprecated, use readdir() instead */
	// int (*getdir) (const char *, fuse_dirh_t, fuse_dirfil_t);

	/** Create a file node
	 *
	 * This is called for creation of all non-directory, non-symlink
	 * nodes.  If the filesystem defines a create() method, then for
	 * regular files that will be called instead.
	 */
	int (*mknod) (const char *, mode_t, dev_t);

	/** Create a directory 
	 *
	 * Note that the mode argument may not have the type specification
	 * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
	 * correct directory type bits use  mode|S_IFDIR
	 * */
	int (*mkdir) (const char *, mode_t);

	/** Remove a file */
	int (*unlink) (const char *);

	/** Remove a directory */
	int (*rmdir) (const char *);

	/** Create a symbolic link */
	int (*symlink) (const char *, const char *);

	/** Rename a file */
	int (*rename) (const char *, const char *);

	/** Create a hard link to a file */
	int (*link) (const char *, const char *);

	/** Change the permission bits of a file */
	int (*chmod) (const char *, mode_t);

	/** Change the owner and group of a file */
	int (*chown) (const char *, uid_t, gid_t);

	/** Change the size of a file */
	int (*truncate) (const char *, off_t);

	/** Change the access and/or modification times of a file
	 *
	 * Deprecated, use utimens() instead.
	 */
	int (*utime) (const char *, struct utimbuf *);

	/** File open operation
	 *
	 * No creation (O_CREAT, O_EXCL) and by default also no
	 * truncation (O_TRUNC) flags will be passed to open(). If an
	 * application specifies O_TRUNC, fuse first calls truncate()
	 * and then open(). Only if 'atomic_o_trunc' has been
	 * specified and kernel version is 2.6.24 or later, O_TRUNC is
	 * passed on to open.
	 *
	 * Unless the 'default_permissions' mount option is given,
	 * open should check if the operation is permitted for the
	 * given flags. Optionally open may also return an arbitrary
	 * filehandle in the fuse_file_info structure, which will be
	 * passed to all file operations.
	 *
	 * Changed in version 2.2
	 */
	int (*open) (const char *, struct fuse_file_info *);

	/** Read data from an open file
	 *
	 * Read should return exactly the number of bytes requested except
	 * on EOF or error, otherwise the rest of the data will be
	 * substituted with zeroes.	 An exception to this is when the
	 * 'direct_io' mount option is specified, in which case the return
	 * value of the read system call will reflect the return value of
	 * this operation.
	 *
	 * Changed in version 2.2
	 */
	int (*read) (const char *, char *, size_t, off_t,
		     struct fuse_file_info *);

	/** Write data to an open file
	 *
	 * Write should return exactly the number of bytes requested
	 * except on error.	 An exception to this is when the 'direct_io'
	 * mount option is specified (see read operation).
	 *
	 * Changed in version 2.2
	 */
	int (*write) (const char *, const char *, size_t, off_t,
		      struct fuse_file_info *);

	/** Get file system statistics
	 *
	 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
	 *
	 * Replaced 'struct statfs' parameter with 'struct statvfs' in
	 * version 2.5
	 */
	int (*statfs) (const char *, struct statvfs *);

	/** Possibly flush cached data
	 *
	 * BIG NOTE: This is not equivalent to fsync().  It's not a
	 * request to sync dirty data.
	 *
	 * Flush is called on each close() of a file descriptor.  So if a
	 * filesystem wants to return write errors in close() and the file
	 * has cached dirty data, this is a good place to write back data
	 * and return any errors.  Since many applications ignore close()
	 * errors this is not always useful.
	 *
	 * NOTE: The flush() method may be called more than once for each
	 * open().	This happens if more than one file descriptor refers
	 * to an opened file due to dup(), dup2() or fork() calls.	It is
	 * not possible to determine if a flush is final, so each flush
	 * should be treated equally.  Multiple write-flush sequences are
	 * relatively rare, so this shouldn't be a problem.
	 *
	 * Filesystems shouldn't assume that flush will always be called
	 * after some writes, or that if will be called at all.
	 *
	 * Changed in version 2.2
	 */
	int (*flush) (const char *, struct fuse_file_info *);

	/** Release an open file
	 *
	 * Release is called when there are no more references to an open
	 * file: all file descriptors are closed and all memory mappings
	 * are unmapped.
	 *
	 * For every open() call there will be exactly one release() call
	 * with the same flags and file descriptor.	 It is possible to
	 * have a file opened more than once, in which case only the last
	 * release will mean, that no more reads/writes will happen on the
	 * file.  The return value of release is ignored.
	 *
	 * Changed in version 2.2
	 */
	int (*release) (const char *, struct fuse_file_info *);

	/** Synchronize file contents
	 *
	 * If the datasync parameter is non-zero, then only the user data
	 * should be flushed, not the meta data.
	 *
	 * Changed in version 2.2
	 */
	int (*fsync) (const char *, int, struct fuse_file_info *);

	/** Set extended attributes */
	int (*setxattr) (const char *, const char *, const char *, size_t, int);

	/** Get extended attributes */
	int (*getxattr) (const char *, const char *, char *, size_t);

	/** List extended attributes */
	int (*listxattr) (const char *, char *, size_t);

	/** Remove extended attributes */
	int (*removexattr) (const char *, const char *);

	/** Open directory
	 *
	 * Unless the 'default_permissions' mount option is given,
	 * this method should check if opendir is permitted for this
	 * directory. Optionally opendir may also return an arbitrary
	 * filehandle in the fuse_file_info structure, which will be
	 * passed to readdir, releasedir and fsyncdir.
	 *
	 * Introduced in version 2.3
	 */
	int (*opendir) (const char *, struct fuse_file_info *);

	/** Read directory
	 *
	 * This supersedes the old getdir() interface.  New applications
	 * should use this.
	 *
	 * The filesystem may choose between two modes of operation:
	 *
	 * 1) The readdir implementation ignores the offset parameter, and
	 * passes zero to the filler function's offset.  The filler
	 * function will not return '1' (unless an error happens), so the
	 * whole directory is read in a single readdir operation.  This
	 * works just like the old getdir() method.
	 *
	 * 2) The readdir implementation keeps track of the offsets of the
	 * directory entries.  It uses the offset parameter and always
	 * passes non-zero offset to the filler function.  When the buffer
	 * is full (or an error happens) the filler function will return
	 * '1'.
	 *
	 * Introduced in version 2.3
	 */
	int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t,
			struct fuse_file_info *);

	/** Release directory
	 *
	 * Introduced in version 2.3
	 */
	int (*releasedir) (const char *, struct fuse_file_info *);

	/** Synchronize directory contents
	 *
	 * If the datasync parameter is non-zero, then only the user data
	 * should be flushed, not the meta data
	 *
	 * Introduced in version 2.3
	 */
	int (*fsyncdir) (const char *, int, struct fuse_file_info *);

	/**
	 * Initialize filesystem
	 *
	 * The return value will passed in the private_data field of
	 * fuse_context to all file operations and as a parameter to the
	 * destroy() method.
	 *
	 * Introduced in version 2.3
	 * Changed in version 2.6
	 */
	void *(*init) (struct fuse_conn_info *conn);

	/**
	 * Clean up filesystem
	 *
	 * Called on filesystem exit.
	 *
	 * Introduced in version 2.3
	 */
	void (*destroy) (void *);

	/**
	 * Check file access permissions
	 *
	 * This will be called for the access() system call.  If the
	 * 'default_permissions' mount option is given, this method is not
	 * called.
	 *
	 * This method is not called under Linux kernel versions 2.4.x
	 *
	 * Introduced in version 2.5
	 */
	int (*access) (const char *, int);

	/**
	 * Create and open a file
	 *
	 * If the file does not exist, first create it with the specified
	 * mode, and then open it.
	 *
	 * If this method is not implemented or under Linux kernel
	 * versions earlier than 2.6.15, the mknod() and open() methods
	 * will be called instead.
	 *
	 * Introduced in version 2.5
	 */
	int (*create) (const char *, mode_t, struct fuse_file_info *);

	/**
	 * Change the size of an open file
	 *
	 * This method is called instead of the truncate() method if the
	 * truncation was invoked from an ftruncate() system call.
	 *
	 * If this method is not implemented or under Linux kernel
	 * versions earlier than 2.6.15, the truncate() method will be
	 * called instead.
	 *
	 * Introduced in version 2.5
	 */
	int (*ftruncate) (const char *, off_t, struct fuse_file_info *);

	/**
	 * Get attributes from an open file
	 *
	 * This method is called instead of the getattr() method if the
	 * file information is available.
	 *
	 * Currently this is only called after the create() method if that
	 * is implemented (see above).  Later it may be called for
	 * invocations of fstat() too.
	 *
	 * Introduced in version 2.5
	 */
	int (*fgetattr) (const char *, struct stat *, struct fuse_file_info *);

	/**
	 * Perform POSIX file locking operation
	 *
	 * The cmd argument will be either F_GETLK, F_SETLK or F_SETLKW.
	 *
	 * For the meaning of fields in 'struct flock' see the man page
	 * for fcntl(2).  The l_whence field will always be set to
	 * SEEK_SET.
	 *
	 * For checking lock ownership, the 'fuse_file_info->owner'
	 * argument must be used.
	 *
	 * For F_GETLK operation, the library will first check currently
	 * held locks, and if a conflicting lock is found it will return
	 * information without calling this method.	 This ensures, that
	 * for local locks the l_pid field is correctly filled in.	The
	 * results may not be accurate in case of race conditions and in
	 * the presence of hard links, but it's unlikely that an
	 * application would rely on accurate GETLK results in these
	 * cases.  If a conflicting lock is not found, this method will be
	 * called, and the filesystem may fill out l_pid by a meaningful
	 * value, or it may leave this field zero.
	 *
	 * For F_SETLK and F_SETLKW the l_pid field will be set to the pid
	 * of the process performing the locking operation.
	 *
	 * Note: if this method is not implemented, the kernel will still
	 * allow file locking to work locally.  Hence it is only
	 * interesting for network filesystems and similar.
	 *
	 * Introduced in version 2.6
	 */
	int (*lock) (const char *, struct fuse_file_info *, int cmd,
		     struct flock *);

	/**
	 * Change the access and modification times of a file with
	 * nanosecond resolution
	 *
	 * This supersedes the old utime() interface.  New applications
	 * should use this.
	 *
	 * See the utimensat(2) man page for details.
	 *
	 * Introduced in version 2.6
	 */
	int (*utimens) (const char *, const struct timespec tv[2]);

	/**
	 * Map block index within file to block index within device
	 *
	 * Note: This makes sense only for block device backed filesystems
	 * mounted with the 'blkdev' option
	 *
	 * Introduced in version 2.6
	 */
	int (*bmap) (const char *, size_t blocksize, uint64_t *idx);

	/**
	 * Flag indicating that the filesystem can accept a NULL path
	 * as the first argument for the following operations:
	 *
	 * read, write, flush, release, fsync, readdir, releasedir,
	 * fsyncdir, ftruncate, fgetattr, lock, ioctl and poll
	 *
	 * If this flag is set these operations continue to work on
	 * unlinked files even if "-ohard_remove" option was specified.
	 */
	unsigned int flag_nullpath_ok:1;

	/**
	 * Flag indicating that the path need not be calculated for
	 * the following operations:
	 *
	 * read, write, flush, release, fsync, readdir, releasedir,
	 * fsyncdir, ftruncate, fgetattr, lock, ioctl and poll
	 *
	 * Closely related to flag_nullpath_ok, but if this flag is
	 * set then the path will not be calculaged even if the file
	 * wasn't unlinked.  However the path can still be non-NULL if
	 * it needs to be calculated for some other reason.
	 */
	unsigned int flag_nopath:1;

	/**
	 * Flag indicating that the filesystem accepts special
	 * UTIME_NOW and UTIME_OMIT values in its utimens operation.
	 */
	unsigned int flag_utime_omit_ok:1;

	/**
	 * Reserved flags, don't set
	 */
	unsigned int flag_reserved:29;

	/**
	 * Ioctl
	 *
	 * flags will have FUSE_IOCTL_COMPAT set for 32bit ioctls in
	 * 64bit environment.  The size and direction of data is
	 * determined by _IOC_*() decoding of cmd.  For _IOC_NONE,
	 * data will be NULL, for _IOC_WRITE data is out area, for
	 * _IOC_READ in area and if both are set in/out area.  In all
	 * non-NULL cases, the area is of _IOC_SIZE(cmd) bytes.
	 *
	 * If flags has FUSE_IOCTL_DIR then the fuse_file_info refers to a
	 * directory file handle.
	 *
	 * Introduced in version 2.8
	 */
	int (*ioctl) (const char *, int cmd, void *arg,
		      struct fuse_file_info *, unsigned int flags, void *data);

	/**
	 * Poll for IO readiness events
	 *
	 * Note: If ph is non-NULL, the client should notify
	 * when IO readiness events occur by calling
	 * fuse_notify_poll() with the specified ph.
	 *
	 * Regardless of the number of times poll with a non-NULL ph
	 * is received, single notification is enough to clear all.
	 * Notifying more times incurs overhead but doesn't harm
	 * correctness.
	 *
	 * The callee is responsible for destroying ph with
	 * fuse_pollhandle_destroy() when no longer in use.
	 *
	 * Introduced in version 2.8
	 */
	int (*poll) (const char *, struct fuse_file_info *,
		     struct fuse_pollhandle *ph, unsigned *reventsp);

	/** Write contents of buffer to an open file
	 *
	 * Similar to the write() method, but data is supplied in a
	 * generic buffer.  Use fuse_buf_copy() to transfer data to
	 * the destination.
	 *
	 * Introduced in version 2.9
	 */
	int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off,
			  struct fuse_file_info *);

	/** Store data from an open file in a buffer
	 *
	 * Similar to the read() method, but data is stored and
	 * returned in a generic buffer.
	 *
	 * No actual copying of data has to take place, the source
	 * file descriptor may simply be stored in the buffer for
	 * later data transfer.
	 *
	 * The buffer must be allocated dynamically and stored at the
	 * location pointed to by bufp.  If the buffer contains memory
	 * regions, they too must be allocated using malloc().  The
	 * allocated memory will be freed by the caller.
	 *
	 * Introduced in version 2.9
	 */
	int (*read_buf) (const char *, struct fuse_bufvec **bufp,
			 size_t size, off_t off, struct fuse_file_info *);
	/**
	 * Perform BSD file locking operation
	 *
	 * The op argument will be either LOCK_SH, LOCK_EX or LOCK_UN
	 *
	 * Nonblocking requests will be indicated by ORing LOCK_NB to
	 * the above operations
	 *
	 * For more information see the flock(2) manual page.
	 *
	 * Additionally fi->owner will be set to a value unique to
	 * this open file.  This same value will be supplied to
	 * ->release() when the file is released.
	 *
	 * Note: if this method is not implemented, the kernel will still
	 * allow file locking to work locally.  Hence it is only
	 * interesting for network filesystems and similar.
	 *
	 * Introduced in version 2.9
	 */
	int (*flock) (const char *, struct fuse_file_info *, int op);

	/**
	 * Allocates space for an open file
	 *
	 * This function ensures that required space is allocated for specified
	 * file.  If this function returns success then any subsequent write
	 * request to specified range is guaranteed not to fail because of lack
	 * of space on the file system media.
	 *
	 * Introduced in version 2.9.1
	 */
	int (*fallocate) (const char *, int, off_t, off_t,
			  struct fuse_file_info *);
};

#define FUSE_ARGS_INIT(argc, argv) { argc, argv, 0 }
struct fuse_args {
	/** Argument count */
	int argc;

	/** Argument vector.  NULL terminated */
	char **argv;

	/** Is 'argv' allocated? */
	int allocated;
};



struct fuse_conn_info {
	/**
	 * Major version of the protocol (read-only)
	 */
	unsigned proto_major;

	/**
	 * Minor version of the protocol (read-only)
	 */
	unsigned proto_minor;

	/**
	 * Maximum size of the write buffer
	 */
	unsigned max_write;

	/**
	 * Maximum size of read requests. A value of zero indicates no
	 * limit. However, even if the filesystem does not specify a
	 * limit, the maximum size of read requests will still be
	 * limited by the kernel.
	 *
	 * NOTE: For the time being, the maximum size of read requests
	 * must be set both here *and* passed to fuse_session_new()
	 * using the ``-o max_read=<n>`` mount option. At some point
	 * in the future, specifying the mount option will no longer
	 * be necessary.
	 */
	unsigned max_read;

	/**
	 * Maximum readahead
	 */
	unsigned max_readahead;

	/**
	 * Capability flags that the kernel supports (read-only)
	 */
	unsigned capable;

	/**
	 * Capability flags that the filesystem wants to enable.
	 *
	 * libfuse attempts to initialize this field with
	 * reasonable default values before calling the init() handler.
	 */
	unsigned want;

	/**
	 * Maximum number of pending "background" requests. A
	 * background request is any type of request for which the
	 * total number is not limited by other means. As of kernel
	 * 4.8, only two types of requests fall into this category:
	 *
	 *   1. Read-ahead requests
	 *   2. Asynchronous direct I/O requests
	 *
	 * Read-ahead requests are generated (if max_readahead is
	 * non-zero) by the kernel to preemptively fill its caches
	 * when it anticipates that userspace will soon read more
	 * data.
	 *
	 * Asynchronous direct I/O requests are generated if
	 * FUSE_CAP_ASYNC_DIO is enabled and userspace submits a large
	 * direct I/O request. In this case the kernel will internally
	 * split it up into multiple smaller requests and submit them
	 * to the filesystem concurrently.
	 *
	 * Note that the following requests are *not* background
	 * requests: writeback requests (limited by the kernel's
	 * flusher algorithm), regular (i.e., synchronous and
	 * buffered) userspace read/write requests (limited to one per
	 * thread), asynchronous read requests (Linux's io_submit(2)
	 * call actually blocks, so these are also limited to one per
	 * thread).
	 */
	unsigned max_background;

	/**
	 * Kernel congestion threshold parameter. If the number of pending
	 * background requests exceeds this number, the FUSE kernel module will
	 * mark the filesystem as "congested". This instructs the kernel to
	 * expect that queued requests will take some time to complete, and to
	 * adjust its algorithms accordingly (e.g. by putting a waiting thread
	 * to sleep instead of using a busy-loop).
	 */
	unsigned congestion_threshold;

	/**
	 * When FUSE_CAP_WRITEBACK_CACHE is enabled, the kernel is responsible
	 * for updating mtime and ctime when write requests are received. The
	 * updated values are passed to the filesystem with setattr() requests.
	 * However, if the filesystem does not support the full resolution of
	 * the kernel timestamps (nanoseconds), the mtime and ctime values used
	 * by kernel and filesystem will differ (and result in an apparent
	 * change of times after a cache flush).
	 *
	 * To prevent this problem, this variable can be used to inform the
	 * kernel about the timestamp granularity supported by the file-system.
	 * The value should be power of 10.  The default is 1, i.e. full
	 * nano-second resolution. Filesystems supporting only second resolution
	 * should set this to 1000000000.
	 */
	unsigned time_gran;

	/**
	 * For future use.
	 */
	unsigned reserved[22];
};


typedef int (*fuse_opt_proc_t)(void *data, const char *arg, int key,
			       struct fuse_args *outargs);


/**
 * Option parsing function
 *
 * If 'args' was returned from a previous call to fuse_opt_parse() or
 * it was constructed from
 *
 * A NULL 'args' is equivalent to an empty argument vector
 *
 * A NULL 'opts' is equivalent to an 'opts' array containing a single
 * end marker
 *
 * A NULL 'proc' is equivalent to a processing function always
 * returning '1'
 *
 * @param args is the input and output argument list
 * @param data is the user data
 * @param opts is the option description array
 * @param proc is the processing function
 * @return -1 on error, 0 on success
 */
int fuse_opt_parse(struct fuse_args *args, void *data,
		   const struct fuse_opt opts[], fuse_opt_proc_t proc);




struct fuse_opt_context {
	void *data;
	const struct fuse_opt *opt;
	fuse_opt_proc_t proc;
	int argctr;
	int argc;
	char **argv;
	struct fuse_args outargs;
	char *opts;
	int nonopt;
};


#define FUSE_OPT_KEY_OPT     -1

/**
 * Key value passed to the processing function for all non-options
 *
 * Non-options are the arguments beginning with a character other than
 * '-' or all arguments after the special '--' option
 */
#define FUSE_OPT_KEY_NONOPT  -2

/**
 * Special key value for options to keep
 *
 * Argument is not passed to processing function, but behave as if the
 * processing function returned 1
 */
#define FUSE_OPT_KEY_KEEP -3

/**
 * Special key value for options to discard
 *
 * Argument is not passed to processing function, but behave as if the
 * processing function returned zero
 */
#define FUSE_OPT_KEY_DISCARD -4
