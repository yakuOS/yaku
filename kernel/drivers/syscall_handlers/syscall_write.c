#include <multitasking/scheduler.h>
#include <multitasking/task.h>


int64_t syscall_write(int fd, const void* buf, size_t count){
    task_t* task = scheduler_get_current_task();
    file_handle_t* file = task->files[fd];
    // serial_printf("syscall_write: filehandle path=%s, operations pointer=%p\n", file->path, file->operations);
    if (file == NULL){
        return -1;
    }
    if (file->operations == NULL || file->operations->write == NULL){
        return -1;
    }
    // serial_printf("syscall_write: fd=%d, buf=%s, count=%d\n", fd, buf, count);
    uint64_t bytes_written = file->operations->write(file->path, buf, count, file->file_byte_ptr, &file->file_handle);
    file->file_byte_ptr += bytes_written;
    // serial_printf("syscall_write: bytes_written=%d\n", bytes_written);
    return bytes_written;
}
