#pragma once
#include <types.h>
#include "create_virtual_file.h"
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
    SYS_create_virtual_directory,
    SYS_open, 
    SYS_write, 
    SYS_puts,// as library that calls write
    SYS_read, 
    SYS_getc,// as library that calls read, 
    SYS_lseek,// - end offset calls getattr and size = end offset, 
    SYS_tell,// as library that calls lseek (lseek returns the new position) , 
    SYS_rewind, 
    SYS_close
};

extern uint64_t syscall(enum syscalls syscall, ...);
