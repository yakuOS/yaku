#pragma once
#include <lib/datetime.h>
#include <lib/origin.h>
#define max_blocks 2000 //block size=4096 bytes * 2000 = 8MB
#define MAX_OPEN_FILES 200
#define MAX_FILENAME_LENGTH 201
#include <lib/stat.h>


// virtual file struct used to write from to disk or read to from disk
typedef struct file {
    struct stat st;
    char name[MAX_FILENAME_LENGTH];
    uint64_t byte_ptr;
    uint8_t* data[max_blocks]; // ptrs to blocks
    char access_mode[3];
} FILE;

struct FILE* fopen(char* filename, char* access_mode);
uint8_t fseek(FILE* drive_image, int64_t offset, enum origin origin);
void rewind(FILE* drive_image);
void fclose(FILE* drive_image);
uint64_t ftell(FILE* drive_image);
uint8_t fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               FILE* image);
uint8_t fread(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
              FILE* image);
uint8_t fputs(char* str, FILE* image);
uint8_t fflush(FILE* image);
uint8_t fgetc(FILE* image);
