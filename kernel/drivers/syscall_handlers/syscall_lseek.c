#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <virtual_fs/virtual_fs.h>
#include <lib/stat.h>
#include <drivers/serial.h>

enum whence { SEEK_SET, SEEK_CUR, SEEK_END };

int64_t syscall_lseek(int fd, int64_t offset, int whence) {
    serial_printf("syscall_lseek: fd=%d, offset=%d, whence=%d\n", fd, offset, whence);
    task_t* task = scheduler_get_current_task();
    file_handle_t* file = task->files[fd];
    if (file == NULL) {
        return -1;
    }
    if (whence == SEEK_SET) {
        if (offset < 0) {
            return -1;
        }
        file->file_byte_ptr = offset;
        return file->file_byte_ptr;
    }
    if (whence == SEEK_CUR) {
        if (offset < 0) {
            if (file->file_byte_ptr < (uint64_t)(-offset)) {
                return -1;
            }
        }
        file->file_byte_ptr += offset;
        return file->file_byte_ptr;
    }
    if (whence == SEEK_END) {
        struct stat st;
        if (file->operations->fgetattr != NULL) {
            serial_printf("syscall_lseek: fgetattr != NULL\n");
            file->operations->fgetattr(file->path, &st, &file->file_handle);
        }
        else if (file->operations->getattr != NULL) {
            serial_printf("syscall_lseek: getattr\n");
            file->operations->getattr(file->path, &st);
        }
        else {
            serial_printf("syscall_lseek: file->operations->fgetattr == NULL && file->operations->getattr == NULL\n");
            return -1;
        }
        uint64_t file_size = st.st_size;
        if (offset < 0) {
            if (file_size < (uint64_t)(-offset)) {
                return -1;
            }
        }
        file->file_byte_ptr = file_size + offset;
        serial_printf("syscall_lseek: file_size=%d, file->file_byte_ptr=%d\n", file_size, file->file_byte_ptr);
        return file->file_byte_ptr;
    }
}
