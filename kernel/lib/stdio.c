#include "stdio.h"
#include <drivers/serial.h>
#include <lib/stat.h>
#include <lib/syscall_wrapper/close.h>
#include <lib/syscall_wrapper/create_virtual_file.h>
#include <lib/syscall_wrapper/getdents.h>
#include <lib/syscall_wrapper/lseek.h>
#include <lib/syscall_wrapper/open.h>
#include <lib/syscall_wrapper/read.h>
#include <lib/syscall_wrapper/write.h>
#include <lib/syscall_wrapper/mknod.h>
#include <types.h>
#include <thirdparty/string/string.h>

int fprintf(FILE* stream, const char* format, ...) {

    return 0;
}

FILE* fopen(const char* filename, const char* mode) {
    serial_printf("fopen: filename=%s\n", filename);
    if (mode[0]=='w'){
        mknod(filename, S_IFREG, 0);
    }
    return open(filename, S_IFREG);
}

int fclose(FILE* stream) {
    return close(stream);
}

int fgetc(FILE* stream) {
    char c;
    read(stream, &c, 1);
    return c;
}

int fputc(int character, FILE* stream) {
    char c = (char)character;
    write(stream, &c, 1);
    return c;
}

int fputs(const char* str, FILE* stream) {
    write(stream, str, strlen(str));
    return 0;
}

int fgets(char* str, int num, FILE* stream) {
    read(stream, str, num);
    return 0;
}

size_t fread(void* ptr, size_t size, size_t count, FILE* stream) {
    return read(stream, ptr, size * count) / size;
}

int fseek(FILE* stream, long int offset, int origin) {
    return lseek(stream, offset, origin);
}

long int ftell(FILE* stream) {
    return lseek(stream, 0, SEEK_CUR);
}

size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
    return write(stream, ptr, size * count) / size;
}

int fflush(FILE* stream) {
    return 0;
}

void rewind(FILE* stream) {
    lseek(stream, 0, SEEK_SET);
}

int get_dir_entries(char* path, struct dir_entries* dir_entries,
                    uint64_t count /*in entries*/) {
    FILE* fd = open(path, S_IFDIR);
    char dirents[count * 200];
    uint64_t b_read = getdents(fd, dirents, count * 200);

    uint64_t files_read = 0;
    for (uint64_t bpos = 0; bpos < count * 200 && bpos < b_read && files_read < count;) {
        struct dirent* dirent = (struct dirent*)&dirents[bpos];
        dir_entries[files_read].is_dir =
            *(mode_t*)(((char*)dirent)[dirent->d_reclen - sizeof(mode_t)]) == S_IFDIR ? true
                                                                                  : false;
        strcpy(dir_entries[files_read].name, &dirent->d_name);
        files_read++;
        bpos += dirent->d_reclen;
    }
    serial_printf("get_dir_entries: files_read=%d \n", files_read);
    close(fd);
    serial_printf("get_dir_entries2: files_read=%d \n", files_read);
}
