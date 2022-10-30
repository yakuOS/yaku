#pragma once
#include <types.h>
typedef uint64_t FILE;
#define stderr 0
#define stdout 1

FILE * fopen ( const char * filename, const char * mode );
int fclose ( FILE * stream );
int fgetc ( FILE * stream );
int fputc ( int character, FILE * stream );
int fputs ( const char * str, FILE * stream );
int fgets ( char * str, int num, FILE * stream );
size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
int fseek ( FILE * stream, long int offset, int origin );
long int ftell ( FILE * stream );
size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
int fflush ( FILE * stream );
void rewind ( FILE * stream );


int fprintf ( FILE * stream, const char * format, ... );

struct dir_entries {
    char name[256];
    bool is_dir;
};
int get_dir_entries(char* path, struct dir_entries* dir_entries, uint64_t count);
