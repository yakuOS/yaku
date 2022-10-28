#include <multitasking/scheduler.h>
#include <multitasking/task.h>


int64_t syscall_write(int fd, const void* buf, size_t count){
    task_t* task = scheduler_get_current_task();
    FILE* file = task->files[fd];
    if (file == NULL){
        return -1;
    }
    if (file->operations->write == NULL){
        return -1;
    }
    uint64_t bytes_written = file->operations->write(file->path, buf, count, file->file_byte_ptr, &file->file_handle);
    file->file_byte_ptr += bytes_written;
    return bytes_written;
}
