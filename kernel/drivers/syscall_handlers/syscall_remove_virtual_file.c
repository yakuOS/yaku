#include "syscall.h"
#include <virtual_fs/virtual_fs.h>

uint64_t syscall_remove_virtual_file(char* path){
    return virtual_fs_remove_endpoint(path);
}
