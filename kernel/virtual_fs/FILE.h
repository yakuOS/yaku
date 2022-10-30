/*
@TODO
file structure
attributes
- file_handle
- fuse_ops set
- file_byte_ptr
- path
*/


#pragma once
#include <types.h>
#include <lib/fuse.h>
struct file_handle {
    struct fuse_file_info file_handle;
    uint64_t file_byte_ptr;
    char path[100];
    struct fuse_operations* operations;
} typedef file_handle_t;
