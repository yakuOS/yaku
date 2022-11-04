#pragma once
#include <types.h>

enum syscalls {
    SYS_create_task,
    SYS_sleep,
    SYS_pause,
    SYS_kill,
    SYS_get_pid,
    SYS_get_ppid,
    SYS_get_time,
    SYS_get_ticks,
    SYS_get_window_buffer,
    SYS_create_virtual_file,
    SYS_remove_virtual_file,
    SYS_create_virtual_directory,
    SYS_open,
    SYS_mknod,
    SYS_getdents, 
    SYS_write,
    SYS_read,
    SYS_lseek,// - end offset calls getattr and size = end offset
    SYS_close,
    SYS_mkdir,
    SYS_rmdir,
    SYS_unlink,
    SYS_get_open_pointer,
};

extern uint64_t syscall(enum syscalls syscall, ...);
