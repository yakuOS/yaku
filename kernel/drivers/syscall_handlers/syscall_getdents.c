#include <memory/pmm.h>
#include <virtual_fs/FILE.h>
#include <virtual_fs/virtual_fs.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <lib/stat.h>
#include <lib/fuse.h>
#include <thirdparty/string/string.h>

struct dirent {
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name;
    mode_t type;
};

struct dirent_buffer_info_pointer{
    struct dirent* dirent_buffer;
    uint64_t buffer_size; // in bytes
    uint64_t max_buffer_size; // in bytes
};

fuse_fill_dir_t syscall_getdents_fill_dir(void* buf, const char* name, const struct stat* stbuf, off_t off, enum fuse_fill_dir_flags flags){
    struct dirent_buffer_info_pointer* dirent_buffer_info_pointer = (struct dirent_buffer_info_pointer*) buf;

    size_t name_length = strlen(name);
    size_t dirent_size = sizeof(struct dirent) + name_length;
    if (dirent_buffer_info_pointer->buffer_size + dirent_size > dirent_buffer_info_pointer->max_buffer_size){
        //buffer full
        return 1;
    }
    dirent_buffer_info_pointer->dirent_buffer->d_ino = stbuf->st_ino;
    dirent_buffer_info_pointer->dirent_buffer->d_off = off;
    dirent_buffer_info_pointer->dirent_buffer->d_reclen = sizeof(struct dirent);
    strcpy(&dirent_buffer_info_pointer->dirent_buffer->d_name, name);
    ((uint8_t*)dirent_buffer_info_pointer->dirent_buffer)[dirent_size - sizeof(mode_t)] = stbuf->st_mode; // copies the mode to the end of the dirent

    dirent_buffer_info_pointer->buffer_size += dirent_size;
    dirent_buffer_info_pointer->dirent_buffer++;
    return 0;
}

int64_t syscall_getdents(int fd, void* dirp, size_t count) {
    FILE* file = scheduler_get_current_task()->files[fd];
    if (file->operations->readdir == NULL) {
        return -1;
    }
    struct dirent_buffer_info_pointer* dirent_buffer_pointer = malloc(sizeof(struct dirent_buffer_info_pointer));
    dirent_buffer_pointer->buffer_size = 0;
    dirent_buffer_pointer->max_buffer_size = count;
    dirent_buffer_pointer->dirent_buffer = dirp;
    file->operations->readdir(file->path, dirent_buffer_pointer, syscall_getdents_fill_dir, 0, &file->file_handle);
}
