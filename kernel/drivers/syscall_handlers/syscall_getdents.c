#include <drivers/serial.h>
#include <lib/fuse.h>
#include <lib/stat.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <thirdparty/string/string.h>
#include <virtual_fs/FILE.h>
#include <virtual_fs/virtual_fs.h>

struct dirent {
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name;
    mode_t type;
};

struct dirent_buffer_info_pointer {
    struct dirent* dirent_buffer;
    uint64_t buffer_size;     // in bytes
    uint64_t max_buffer_size; // in bytes
    file_handle_t* file;
};

fuse_fill_dir_t syscall_getdents_fill_dir(void* buf, const char* name,
                                          const struct stat* stbuf, off_t off,
                                          enum fuse_fill_dir_flags flags) {
                                            serial_printf("syscall_getdents_fill_dir: name=%s, stbuf->st_mode=%d, off=%d, flags=%d\n", name, stbuf->st_mode, off, flags);
    struct dirent_buffer_info_pointer* dirent_buffer_info_pointer =
        (struct dirent_buffer_info_pointer*)buf;
    size_t name_length = strlen(name);
    size_t dirent_size = sizeof(struct dirent) + name_length;
    if (dirent_buffer_info_pointer->buffer_size + dirent_size >
        dirent_buffer_info_pointer->max_buffer_size) {
        // buffer full
        return 1;
    }

    dirent_buffer_info_pointer->dirent_buffer->d_ino = stbuf->st_ino;
    dirent_buffer_info_pointer->dirent_buffer->d_off = off;
    dirent_buffer_info_pointer->dirent_buffer->d_reclen = dirent_size;
    strcpy(&dirent_buffer_info_pointer->dirent_buffer->d_name, name);

    struct stat stat;
    if (stbuf == NULL) {
        char file_path[100];
        strcpy(file_path, dirent_buffer_info_pointer->file->path);
        if (file_path[strlen(file_path) - 1] != '/') {
            strcat(file_path, "/");
        }
        strcat(file_path, name);
        dirent_buffer_info_pointer->file->operations->getattr(file_path, &stat);
        stbuf = &stat;
    }
    uint8_t* mode_pointer =
        ((uint8_t*)
             dirent_buffer_info_pointer->dirent_buffer)[dirent_size - sizeof(mode_t)];
    *(mode_t*)mode_pointer = stbuf->st_mode;
    dirent_buffer_info_pointer->buffer_size += dirent_size;
    dirent_buffer_info_pointer->dirent_buffer =
        (struct dirent*)((uint64_t)dirent_buffer_info_pointer->dirent_buffer +
                         (uint64_t)dirent_size);
    return 0;
}

int64_t syscall_getdents(int fd, void* dirp, size_t count) {
    file_handle_t* file = scheduler_get_current_task()->files[fd];
    struct dirent_buffer_info_pointer* dirent_buffer_pointer =
        malloc(sizeof(struct dirent_buffer_info_pointer));
    dirent_buffer_pointer->buffer_size = 0;
    dirent_buffer_pointer->max_buffer_size = count;
    dirent_buffer_pointer->dirent_buffer = dirp;
    dirent_buffer_pointer->file = file;
    // file->operations->readdir(file->path, dirent_buffer_pointer,
    //                           syscall_getdents_fill_dir, 0, &file->file_handle);

    serial_printf("file operations pointer: %x\n", file->operations);
    if (file->operations == NULL) { // should guarantee that directory is part of a virtual_endpoint, if it's not, call the virtual_fs_readdir
        virtual_fs_readdir(file->path, dirent_buffer_pointer, syscall_getdents_fill_dir,
                           0, &file->file_handle);
        // serial_printf("syscall_getdents: %s\n", file->path);
    } else {
        file->operations->readdir(file->path, dirent_buffer_pointer,
                                  syscall_getdents_fill_dir, 0, &file->file_handle);
    }
    serial_printf("syscall_getdents: %s\n", file->path);

    uint64_t size = dirent_buffer_pointer->buffer_size;
    free(dirent_buffer_pointer);
    serial_printf("syscall_getdents3: %s\n", file->path);
    return size;
}
