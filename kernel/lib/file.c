// #include "file.h"
// #include <types.h>
// #include <echfs/echfs-fuse.h>
// #include <drivers/serial.h>

// FILE* fopen(const char* filepath, const char* access_mode) {
//     if (filepath == NULL || access_mode == NULL ||
//         strlen(filepath) > (MAX_FILENAME_LENGTH - 1) || strlen(access_mode) > 2) {
//         return NULL;
//     }
//     FILE* file = (FILE*)malloc((sizeof(FILE) - 1) / 4069 + 1);
//     memset(file, 0, sizeof(FILE));
//     strcpy(file->access_mode, access_mode);
//     file->file_path_len = strlen(filepath);
//     file->file_path = (char*)malloc((file->file_path_len-1)/4069+1);
//     strcpy(file->file_path, filepath);
//     file->byte_ptr = 0;
//     uint64_t tv_sec = (uint64_t)datetime_get_timestamp();
//     file->st.st_mtim.tv_sec = tv_sec;
//     file->st.st_ctim.tv_sec = tv_sec;
//     file->st.st_atim.tv_sec = tv_sec;
    
//     file->st.st_size = 0;
//     file->st.st_mode = 0;
//     struct fuse_file_info *fuse_info = (struct fuse_file_info*)malloc((sizeof(struct fuse_file_info)-1)/4069+1);
//     file->file_info = fuse_info;
    
//     echfs_get_fuse_operations(&file->operations);

//     serial_printf("fopen: %s %s\n", filepath, access_mode);
//     int status = file->operations.open(filepath, fuse_info);
    
//     if (status == -ENOENT){
//         serial_printf("File not found\n");
//         file->operations.create(filepath, access_mode[0], fuse_info);
//     }
//     // if (file->operations.open(filepath, fuse_info) != 0) {
//     //     free(file, (sizeof(FILE) - 1) / 4069 + 1);
//     //     free(fuse_info, (sizeof(struct fuse_file_info)-1)/4069+1);
//     //     return NULL;
//     // }
    
//     return file;
// }


// void mkdir(const char* filepath, mode_t mode) {
//     if (filepath == NULL) {
//         return;
//     }
//     struct fuse_operations operations;
//     echfs_get_fuse_operations(&operations);
//     operations.mkdir(filepath, mode);
// }
