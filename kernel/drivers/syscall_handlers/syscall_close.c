#include <multitasking/scheduler.h>
#include <multitasking/task.h>

int syscall_close(int fd){
    task_t* task = scheduler_get_current_task();
    FILE* file = task->files[fd];
    if (file == NULL){
        return -1;
    }
    if (file->operations->release == NULL){
        task_remove_file(task, fd);
        return -1;
    }
    file->operations->release(file->path, &file->file_handle);
    task_remove_file(task, fd);
    return 0;
}
