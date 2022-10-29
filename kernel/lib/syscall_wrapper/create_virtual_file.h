#pragma once
#include <lib/fuse.h>
#include "syscalls.h"
enum endpoint_type_l { ENDPOINT_TYPE_DIRECTORY_U, ENDPOINT_TYPE_FILE_U };

static inline uint64_t create_virtual_file(struct fuse_operations* operations, enum endpoint_type_l endpoint_type, char* path){
    return syscall(SYS_create_virtual_file, operations, (uint64_t)endpoint_type, path);
}
