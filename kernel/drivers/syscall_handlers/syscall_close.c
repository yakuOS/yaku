#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <drivers/serial.h>

int syscall_close(int fd){
    task_t* task = scheduler_get_current_task();
    file_handle_t* file = task->files[fd];
    serial_printf("syscall_close: filehandle path=%s, operations pointer=%p\n", file->path, file->operations);
    if (file == NULL){
        return -1;
    }
    if (file->operations == NULL ){
        task_remove_file(task, fd);
        return 0;
    }
    if (file->operations->release == NULL){
        task_remove_file(task, fd);
        return -1;
    }
    file->operations->release(file->path, &file->file_handle);
    task_remove_file(task, fd);
    return 0;
}
