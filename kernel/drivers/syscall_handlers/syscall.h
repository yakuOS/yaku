#pragma once
#include <types.h>
#include <lib/fuse.h>
#include <virtual_fs/virtual_fs.h>
#include <multitasking/task.h>

uint64_t isr_syscall(uint64_t rid, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);


uint64_t syscall_create_virtual_file(struct fuse_operations* operations, enum endpoint_type endpoint_type, char* path);
uint64_t syscall_remove_virtual_file(char* path);
int64_t syscall_open(const char* path, int flags);
uint64_t syscall_create_task(void* function, task_parameters_t* task_parameters);
int64_t syscall_getdents(int fd, void* dirp, size_t count);
int64_t syscall_write(int fd, const void* buf, size_t count);
int64_t syscall_read(int fd, void* buf, size_t count);
int64_t syscall_lseek(int fd, int64_t offset, int whence);
int64_t syscall_close(int fd);
