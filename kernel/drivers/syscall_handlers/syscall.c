#include "syscall.h"
#include <drivers/serial.h>
#include <virtual_fs/virtual_fs.h>
#include <multitasking/scheduler.h>
#include <lib/syscall_wrapper/syscalls.h>
#include <memory/pmm.h>
#include <thirdparty/string/string.h>


uint64_t syscall_create_task(void* function, task_parameters_t* task_parameters){
    task_t* task = task_add(function, &task_parameters, TASK_PRIORITY_LOW, 0);
    if (task == NULL) {
        return -1;
    }
    serial_printf("task_add returned %lu\n", (uint64_t)task);
    return task->pid;
}

uint64_t syscall_create_virtual_file(struct fuse_operations* operations, enum endpoint_type endpoint_type, char* path){
    serial_printf("syscall_create_virtual_file\n");
    return virtual_fs_create_endpoint(operations, endpoint_type, path);
}

int64_t syscall_open(const char* path){
    FILE* file = (FILE*)malloc(sizeof(FILE));
    file->file_byte_ptr = 0;
    virtual_fs_open(path, &file->file_handle, file->operations, file->path);
    int32_t file_descriptor = task_add_file(scheduler_get_current_task(), file);
    return file_descriptor;
}



uint64_t isr_syscall(uint64_t rid, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    enum syscalls syscall = (enum syscalls) rid;
    serial_printf("Syscall: %d\n", rid);
    switch (syscall)
    {
    case SYS_create_task:
        return syscall_create_task((void*) rsi, (task_parameters_t*) rdx);
        break;
    case SYS_open:
        return syscall_open((const char*) rsi);
        break;
    case SYS_create_virtual_file:
        return syscall_create_virtual_file((struct fuse_operations*) rsi, (enum endpoint_type) rdx, (char*) rcx);
        break;
    default:
        break;
    }
    return;
}
