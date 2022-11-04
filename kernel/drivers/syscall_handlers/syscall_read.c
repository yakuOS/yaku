#include <multitasking/scheduler.h>
#include <multitasking/task.h>


int64_t syscall_read(int fd, void* buf, size_t count){
    task_t* task = scheduler_get_current_task();
    file_handle_t* file = task->files[fd];
    if (file == NULL){
        return -1;
    }
    if (file->operations == NULL || file->operations->read == NULL){
        return -1;
    }
    uint64_t bytes_written = file->operations->read(file->path, buf, count, file->file_byte_ptr, &file->file_handle);
    file->file_byte_ptr += bytes_written;
    return bytes_written;
}
