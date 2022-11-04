#include "syscall.h"
#include <drivers/serial.h>
#include <lib/syscall_wrapper/syscalls.h>
#include <virtual_fs/virtual_fs.h>





uint64_t isr_syscall(uint64_t rid, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    enum syscalls syscall = (enum syscalls) rid;
    switch (syscall)
    {
    case SYS_create_task:
        return syscall_create_task((void*) rsi, (task_parameters_t*) rdx);
        break;
    case SYS_create_virtual_file:
        return syscall_create_virtual_file((struct fuse_operations*) rsi, (enum endpoint_type) rdx, (char*) rcx);
        break;
    case SYS_remove_virtual_file:
        return syscall_remove_virtual_file((char*) rsi);
        break;
    case SYS_open:
        return syscall_open((const char*) rsi, (int) rdx);
        break;
    case SYS_getdents:
        return syscall_getdents((int) rsi, (void*) rdx, (size_t) rcx);
        break;
    case SYS_write:
        return syscall_write((int) rsi, (const void*) rdx, (size_t) rcx);
        break;
    case SYS_read:
        return syscall_read((int) rsi, (void*) rdx, (size_t) rcx);
        break;
    case SYS_close:
        return syscall_close((int) rsi);
        break;
    case SYS_lseek:
        return syscall_lseek((int) rsi, (off_t) rdx, (int) rcx);
        break;
    case SYS_mkdir:
        return syscall_mkdir((const char*) rsi, (mode_t) rdx);
        break;
    case SYS_rmdir:
        return syscall_rmdir((const char*) rsi);
        break;
    case SYS_unlink:
        return syscall_unlink((const char*) rsi);
        break;
    case SYS_mknod:
        return syscall_mknod((const char*) rsi, (mode_t) rdx, (dev_t) rcx);
        break;
    case SYS_get_open_pointer:
        get_open_pointer_fs();
    default:
        break;
    }
    return;
}
