#include "syscall.h"
#include <virtual_fs/virtual_fs.h>
#include <thirdparty/string/string.h>

uint64_t syscall_create_virtual_file(struct fuse_operations* operations, enum endpoint_type endpoint_type, char* path){
    serial_printf("syscall_create_virtual_file: %s\n", path);
    char path_copy[strlen(path)+1];
    strcpy(path_copy, path);
    if (operations->init != NULL) {
        operations->init(NULL);
    }
    return virtual_fs_create_endpoint(operations, endpoint_type, path_copy);
}
