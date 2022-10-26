#include "virtual_file.h"
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <lib/datetime.h>
#include <memory/pmm.h>
#include <string.h>

static FILE* open_files[MAX_OPEN_FILES];
struct FILE* fopen(char* filename, char* access_mode) {
    if (filename == NULL || access_mode == NULL ||
        strlen(filename) > (MAX_FILENAME_LENGTH - 1) || strlen(access_mode) > 2) {
        return NULL;
    }
    for (uint64_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] != NULL) {
            if (strcmp(open_files[i]->name, filename) == 0) {
                return open_files[i];
            }
        }
    }
    FILE* file = (FILE*)malloc((sizeof(FILE) - 1) / 4069 + 1);
    memset(file, 0, sizeof(FILE));
    strcpy(file->access_mode, access_mode);
    strcpy(file->name, filename);
    file->byte_ptr = 0;
    uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
    file->st.st_mtim.tv_sec = tv_sec;
    file->st.st_ctim.tv_sec = tv_sec;
    file->st.st_atim.tv_sec = tv_sec;
    file->st.st_size = 0;
    file->st.st_mode = 0;
    for (uint64_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == NULL) {
            open_files[i] = file;
            return file;
        }
    }
    return file;
}
uint8_t stat(char* filename, struct stat* st) {
    if (filename == NULL || st == NULL) {
        return 1;
    }
    for (uint64_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (strcmp(open_files[i]->name, filename) == 0) {
            st->st_mode = open_files[i]->st.st_mode;
            st->st_mtim.tv_sec = open_files[i]->st.st_mtim.tv_sec;
            st->st_ctim.tv_sec = open_files[i]->st.st_ctim.tv_sec;
            st->st_atim.tv_sec = open_files[i]->st.st_atim.tv_sec;
            st->st_size = open_files[i]->st.st_size;
            return 0;
        }
    }
    return 1;
}
uint8_t fseek(FILE* drive_image, int64_t offset, enum origin origin) {
    if (origin == SEEK_SET) {
        if (offset < 0)
            return 1;
        drive_image->byte_ptr = offset;
    } else if (origin == SEEK_CUR) {
        if (drive_image->byte_ptr + offset < 0) {
            return 1;
        }
        drive_image->byte_ptr += offset;
    } else if (origin == SEEK_END) {
        if (drive_image->st.st_size + offset < 0) {
            return 1;
        }
        drive_image->byte_ptr = drive_image->st.st_size + offset;
    }
    return 0;
}
void rewind(FILE* file) {
    file->byte_ptr = 0;
}
void fclose(FILE* file) {
    if (file == NULL) {
        return;
    }
    for (int i = 0; i < max_blocks; i++) {
        if (file->data[i] != NULL) {
            free(file->data[i]);
        }
    }
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == file) {
            open_files[i] = NULL;
        }
    }
    free(file);
}
uint64_t ftell(FILE* drive_image) {
    return drive_image->byte_ptr;
}
uint8_t fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               FILE* file) {
    if (file->access_mode[0] != 'w') {
        return 1;
    }
    uint64_t size = size_of_element * number_of_elements;
    uint64_t block_num = file->byte_ptr / 4096;
    uint64_t block_offset = file->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size + block_offset; i += 1) {
        if (file->data[block_num] == NULL) {
            file->data[block_num] = (uint8_t*)malloc(4096);
        }
        file->data[block_num][i % 4096] = ptr[i - block_offset];
        file->byte_ptr += 1;
        if (file->byte_ptr > file->st.st_size) {
            file->st.st_size = file->byte_ptr;
        }
        if (file->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
    file->st.st_mtim.tv_sec = tv_sec;
    file->st.st_ctim.tv_sec = tv_sec;
    file->st.st_atim.tv_sec = tv_sec;
    return 0;
}
uint8_t fputs(char* str, FILE* file) {
    if (file->access_mode[0] != 'w') {
        return 1;
    }
    uint64_t size = strlen(str);
    uint64_t block_num = file->byte_ptr / 4096;
    uint64_t block_offset = file->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size + block_offset; i += 1) {
        if (file->data[block_num] == NULL) {
            file->data[block_num] = (uint8_t*)malloc(4096);
        }
        file->data[block_num][i % 4096] = str[i - block_offset];
        file->byte_ptr += 1;
        if (file->byte_ptr > file->st.st_size) {
            file->st.st_size = file->byte_ptr;
        }
        if (file->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
    file->st.st_mtim.tv_sec = tv_sec;
    file->st.st_ctim.tv_sec = tv_sec;
    file->st.st_atim.tv_sec = tv_sec;
    return 0;
}
uint8_t fread(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
              FILE* file) {

    uint64_t size = size_of_element * number_of_elements;
    uint64_t block_num = file->byte_ptr / 4096;
    uint64_t block_offset = file->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size + block_offset; i += 1) {
        if (file->data[block_num] == NULL) {
            ptr[i - block_offset] = 0;
            file->byte_ptr += 1;
            continue;
        }
        ptr[i - block_offset] = file->data[block_num][i % 4096];
        file->byte_ptr += 1;
        if (file->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
    file->st.st_ctim.tv_sec = tv_sec;
    file->st.st_atim.tv_sec = tv_sec;
    return 0;
}
uint8_t fflush(FILE* file) {
    return 0;
}
uint8_t fgetc(FILE* file) {
    uint64_t block_num = file->byte_ptr / 4096;
    uint64_t block_offset = file->byte_ptr % 4096;
    if (file->data[block_num] == NULL) {
        return 0;
    }
    file->byte_ptr += 1;
    uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
    file->st.st_ctim.tv_sec = tv_sec;
    file->st.st_atim.tv_sec = tv_sec;
    return file->data[block_num][block_offset];
}
