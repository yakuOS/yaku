#pragma once
#include <lib/fuse.h>
#include "syscalls.h"
enum endpoint_type_U { ENDPOINT_TYPE_DIRECTORY_U, ENDPOINT_TYPE_FILE_U };

static inline void create_virtual_file(struct fuse_operations* operations, enum endpoint_type_U endpoint_type, char* path){
    syscall(SYS_create_virtual_file, operations, endpoint_type, path);
}
