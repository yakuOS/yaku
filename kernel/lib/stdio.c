#include <types.h>
#include <lib/syscall_wrapper/open.h>
#include <lib/syscall_wrapper/read.h>
#include <lib/syscall_wrapper/write.h>
#include <lib/syscall_wrapper/close.h>
#include <lib/syscall_wrapper/lseek.h>
#include <lib/syscall_wrapper/getdents.h>
#include <lib/syscall_wrapper/create_virtual_file.h>
#include <drivers/serial.h>
#include <printf.h>
#include <stdarg.h>
#include "stdio.h"

int fprintf ( FILE * stream, const char * format, ... ){
    
    return 0;
}

FILE * fopen ( const char * filename, const char * mode ){
    return open(filename, mode);
}

int fclose ( FILE * stream ){
    return close(stream);
}

int fgetc ( FILE * stream ){
    char c;
    read(stream, &c, 1);
    return c;
}

int fputc ( int character, FILE * stream ){
    char c = (char) character;
    write(stream, &c, 1);
    return c;
}

int fputs ( const char * str, FILE * stream ){
    write(stream, str, strlen(str));
    return 0;
}

int fgets ( char * str, int num, FILE * stream ){
    read(stream, str, num);
    return 0;
}

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream ){
    return read(stream, ptr, size*count)/size;
}

int fseek ( FILE * stream, long int offset, int origin ){
    return lseek(stream, offset, origin);
}

long int ftell ( FILE * stream ){
    return lseek(stream, 0, SEEK_CUR);
}

size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ){
    return write(stream, ptr, size*count)/size;
}

int fflush ( FILE * stream ){
    return 0;
}

void rewind ( FILE * stream ){
    lseek(stream, 0, SEEK_SET);
}
