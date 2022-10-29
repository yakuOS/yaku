#include "syscall.h"
#include <virtual_fs/virtual_fs.h>

uint64_t syscall_create_virtual_file(struct fuse_operations* operations, enum endpoint_type endpoint_type, char* path){
    serial_printf("syscall_create_virtual_file\n");
    if (operations->init != NULL) {
        operations->init(NULL);
    }
    return virtual_fs_create_endpoint(operations, endpoint_type, path);
}
