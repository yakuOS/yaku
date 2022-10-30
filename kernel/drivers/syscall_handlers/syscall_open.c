#include <memory/pmm.h>
#include <virtual_fs/virtual_fs.h>
#include <virtual_fs/FILE.h>
#include <drivers/serial.h>
#define O_DIRECTORY 0b1

int64_t syscall_open(const char* path, int flags){
    file_handle_t* file = (file_handle_t*)malloc(sizeof(file_handle_t));
    file->file_byte_ptr = 0;
    if (flags & O_DIRECTORY !=0){ // check if the directory flag is set
        virtual_fs_opendir(path, file, file->operations, file->path);
    }
    else {
        virtual_fs_open(path, &file->file_handle, file->operations, file->path);
    }
    int32_t file_descriptor = task_add_file(scheduler_get_current_task(), file);
    serial_printf("syscall_open: %s, file descriptor: %d\n", file->path, file_descriptor);
    return file_descriptor;
}
