#include "write_to_drive.h"
#include <drivers/lba/lba.h>
#include <drivers/serial.h>
#include <lib/stat.h>
#include <lib/syscall_wrapper/create_virtual_file.h>
#include <memory/pmm.h>
#include <thirdparty/string/string.h>

const char* drive_first_string = "drive_first";
const char* drive_second_string = "drive_second";
const char* drive_third_string = "drive_third";
const char* drive_fourth_string = "drive_fourth";

struct drive_image drive_images[100];

int16_t get_handle() {
    for (int i = 0; i < 100; i++) {
        if (drive_images[i].occupied == 0) {
            drive_images[i].occupied = 1;
            return i;
        }
    }
    return -1;
}

int write_to_drive_open(const char* drive, struct fuse_file_info* fh) {
    if (drive[0] == '/')
        drive++;
    enum drive drive_to_open;
    // check if drive is recognized
    if (strcmp(drive, drive_first_string) == 0) {
        drive_to_open = drive_first;
        if (!drive_present(primary_controller, first_drive)) {
            return NULL;
        }
    } else if (strcmp(drive, drive_second_string) == 0) {
        drive_to_open = drive_second;
        if (!drive_present(primary_controller, second_drive)) {
            return NULL;
        }
    } else if (strcmp(drive, drive_third_string) == 0) {
        drive_to_open = drive_third;
        if (!drive_present(secondary_controller, first_drive)) {
            return NULL;
        }
    } else if (strcmp(drive, drive_fourth_string) == 0) {
        drive_to_open = drive_fourth;
        if (!drive_present(secondary_controller, second_drive)) {
            return NULL;
        }
    } else {
        return NULL;
    }
    int16_t handle = get_handle();
    if (handle == -1) {
        return -1;
    }
    struct drive_image* drive_image = &drive_images[handle];
    drive_image->drive = drive_to_open;
    drive_image->access_mode = W;
    return 0;
}

void write_to_drive_release(const char* path, struct fuse_file_info* fh) {
    memset(&drive_images[fh->fh], 0, sizeof(struct drive_image));
}

// uint8_t write_to_drive_fseek(struct drive_image* drive_image, int64_t offset /*in
// bytes*/,
//                              enum origin origin) {
//     if (origin == SEEK_SET) {
//         if (offset < 0) {
//             return 0;
//         }
//         drive_image->byte_pointer_position = offset;
//         return 1;
//     } else if (origin == SEEK_CUR) {
//         if (drive_image->byte_pointer_position + offset < 0) {
//             return 0;
//         }
//         drive_image->byte_pointer_position += offset;
//         return 1;
//     } else if (origin = SEEK_END) {
//         if (drive_image->drive == drive_first) {
//             // check if drive is present
//             if (!drive_present(primary_controller, first_drive)) {
//                 return 0;
//             }
//             // check if new pointer would be < 0
//             if (get_drive_size(primary_controller, first_drive) * 512 + offset < 0) {
//                 return 0;
//             }
//             drive_image->byte_pointer_position =
//                 get_drive_size(primary_controller, first_drive) * 512 + offset;
//             return 1;
//         } else if (drive_image->drive == drive_second) {
//             if (!drive_present(primary_controller, second_drive)) {
//                 return 0;
//             }
//             if (get_drive_size(primary_controller, second_drive) * 512 + offset < 0) {
//                 return 0;
//             }
//             drive_image->byte_pointer_position =
//                 get_drive_size(primary_controller, second_drive) * 512 + offset;
//             return 1;
//         } else if (drive_image->drive == drive_third) {
//             if (!drive_present(secondary_controller, first_drive)) {
//                 return 0;
//             }
//             if (get_drive_size(secondary_controller, first_drive) * 512 + offset < 0) {
//                 return 0;
//             }
//             drive_image->byte_pointer_position =
//                 get_drive_size(secondary_controller, first_drive) * 512 + offset;
//             return 1;
//         } else if (drive_image->drive == drive_fourth) {
//             if (!drive_present(secondary_controller, second_drive)) {
//                 return 0;
//             }
//             if (get_drive_size(secondary_controller, second_drive) * 512 + offset < 0)
//             {
//                 return 0;
//             }
//             drive_image->byte_pointer_position =
//                 get_drive_size(secondary_controller, second_drive) * 512 + offset;
//             return 1;
//         }
//     }
//     return 0;
// }


// TODO: for both, write and read, only read/write 255 sectors at a time (seems to work on qemu but shouldn't on real hardware)
int write_to_drive_write(const char* path, const char* buf, size_t size, off_t offset,
                         struct fuse_file_info* fh) {
    // serial_printf("write_to_drive_write\n");
    struct drive_image* image = &drive_images[fh->fh];

    // if (image->access_mode != W) {
    //     return 0;
    // }
    uint64_t byte_to_write_to = (uint64_t)offset;
    uint64_t sector_to_write_to = byte_to_write_to / 512;
    uint64_t byte_in_sector_to_write_to = byte_to_write_to % 512;

    uint8_t* ptr = (uint8_t*)buf;

    if (size % 512 == 0) { // if the size of the element is a multiple of
                           // 512 just write the buffer to the drive
        if (image->drive == drive_first) {

            write_ata_primary_controller_first_drive(sector_to_write_to, size / 512, ptr);
        } else if (image->drive == drive_second) {
            write_ata_primary_controller_second_drive(sector_to_write_to, size / 512,
                                                      ptr);
        } else if (image->drive == drive_third) {
            write_ata_secondary_controller_first_drive(sector_to_write_to, size / 512,
                                                       ptr);
        } else if (image->drive == drive_fourth) {
            write_ata_secondary_controller_second_drive(sector_to_write_to, size / 512,
                                                        ptr);
        }
    } else { // else read the sector from the drive, add the buffer to it and write it
             // back to the drive
        uint8_t
            buffer[(size) + (512 - ((size) % 512))]; // buffer with size of
                                                     // element*number_of_elements rounded
                                                     // up to 512 (sector size)
        if (image->drive == drive_first) {
            read_ata_primary_controller_first_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer); // size / 512+1->
                                   // because size % 512
                                   // != 0, we need to read one sector more than
                                   // size
        } else if (image->drive == drive_second) {
            read_ata_primary_controller_second_drive(sector_to_write_to, size / 512 + 1,
                                                     (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            read_ata_secondary_controller_first_drive(sector_to_write_to, size / 512 + 1,
                                                      (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            read_ata_secondary_controller_second_drive(sector_to_write_to, size / 512 + 1,
                                                       (uint8_t*)buffer);
        }
        for (uint64_t i = byte_in_sector_to_write_to;
             i < byte_in_sector_to_write_to + size; i++) {
            buffer[i] = ptr[i - byte_in_sector_to_write_to];
        }
        if (image->drive == drive_first) {
            write_ata_primary_controller_first_drive(sector_to_write_to, size / 512 + 1,
                                                     (uint8_t*)buffer);
        } else if (image->drive == drive_second) {
            write_ata_primary_controller_second_drive(sector_to_write_to, size / 512 + 1,
                                                      (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            write_ata_secondary_controller_first_drive(sector_to_write_to, size / 512 + 1,
                                                       (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            write_ata_secondary_controller_second_drive(sector_to_write_to,
                                                        size / 512 + 1, (uint8_t*)buffer);
        }
    }
    // serial_printf("write to drive size %d offset %d\n", size, offset);
    return size;
}

uint8_t write_to_drive_fflush(struct drive_image* image) {
    return 1;
}
int write_to_drive_read(const char* path, char* buf, size_t to_read, off_t offset,
                        struct fuse_file_info* file_info) {
    serial_printf("write_to_drive_read\n");
    struct drive_image* image = &drive_images[file_info->fh];
    uint64_t byte_to_read_from = (uint64_t)offset;
    uint64_t sector_to_read_from = byte_to_read_from / 512;
    uint64_t byte_in_sector_to_read_from = byte_to_read_from % 512;

    uint64_t sectors_to_read = 0;
    if (to_read % 512 == 0) {
        sectors_to_read = to_read / 512;
        serial_printf("sectors to read %d \n", sectors_to_read);
        if (image->drive == drive_first) {
            read_ata_primary_controller_first_drive(sector_to_read_from, sectors_to_read,
                                                    (uint8_t*)buf);
        } else if (image->drive == drive_second) {
            read_ata_primary_controller_second_drive(sector_to_read_from, sectors_to_read,
                                                     (uint8_t*)buf);
        } else if (image->drive == drive_third) {
            read_ata_secondary_controller_first_drive(sector_to_read_from,
                                                      sectors_to_read, (uint8_t*)buf);
        } else if (image->drive == drive_fourth) {
            read_ata_secondary_controller_second_drive(sector_to_read_from,
                                                       sectors_to_read, (uint8_t*)buf);
        }
        return to_read;
    } else {
        sectors_to_read = to_read / 512 + 1;
        uint8_t* buffer /*[(size_of_element * number_of_elements) +
                    (512 - ((size_of_element * number_of_elements) %
                            512))]*/
            = (uint8_t*)malloc(
                (to_read) +
                (512 -
                 ((to_read) % 512))); // buffer with size of element*number_of_elements
                                      // rounded up to 512 (sector size)
        if (image->drive == drive_first) {
            read_ata_primary_controller_first_drive(sector_to_read_from, sectors_to_read,
                                                    (uint8_t*)buffer);
        } else if (image->drive == drive_second) {
            read_ata_primary_controller_second_drive(sector_to_read_from, sectors_to_read,
                                                     (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            read_ata_secondary_controller_first_drive(sector_to_read_from,
                                                      sectors_to_read, (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            read_ata_secondary_controller_second_drive(sector_to_read_from,
                                                       sectors_to_read, (uint8_t*)buffer);
        }

        // copy the data from where to read from to the buffer
        for (uint64_t i = byte_in_sector_to_read_from;
             i < byte_in_sector_to_read_from + to_read; i++) {
            buf[i - byte_in_sector_to_read_from] = buffer[i];
        }
        free(buffer);
        return to_read;
    }
}

int write_to_drive_readdir(const char* path, void* buf, fuse_fill_dir_t fill,
                           off_t offset, struct fuse_file_info* file_info) {

    if (strlen(path) > 0 && strcmp(path, "/") != 0) {
        return -1;
    }
    if (offset == 0 && drive_present(primary_controller, first_drive)) {
        fill(buf, "first_drive", NULL, 0, 0);
    }
    if (offset < 2 && drive_present(primary_controller, second_drive)) {
        fill(buf, "second_drive", NULL, 1, 0);
    }
    if (offset < 3 && drive_present(secondary_controller, first_drive)) {
        fill(buf, "third_drive", NULL, 2, 0);
    }
    if (offset < 4 && drive_present(secondary_controller, second_drive)) {
        fill(buf, "fourth_drive", NULL, 3, 0);
    }
    return 0;
}

int write_to_drive_fgetattr(const char* path, struct stat* stat,
                            struct fuse_file_info* file_info) {
    struct drive_image* image = &drive_images[file_info->fh];
    stat->st_blksize = 512;

    if (image->drive == drive_first) {
        stat->st_size = get_drive_size(primary_controller, first_drive) * 512;
        stat->st_blksize = 512;
        stat->st_blocks = stat->st_size / 512;
    } else if (image->drive == drive_second) {
        stat->st_size = get_drive_size(primary_controller, second_drive) * 512;
        stat->st_blksize = 512;
        stat->st_blocks = stat->st_size / 512;
    } else if (image->drive == drive_third) {
        stat->st_size = get_drive_size(secondary_controller, first_drive) * 512;
        stat->st_blksize = 512;
        stat->st_blocks = stat->st_size / 512;
    } else if (image->drive == drive_fourth) {
        stat->st_size = get_drive_size(secondary_controller, second_drive) * 512;
        stat->st_blksize = 512;
        stat->st_blocks = stat->st_size / 512;
    }
}
int write_to_drive_opendir(const char* dir_path, struct fuse_file_info* file_info) {
    return 0;
}

static struct fuse_operations operations = {
    .open = write_to_drive_open,
    .opendir = write_to_drive_opendir,
    .fgetattr = write_to_drive_fgetattr,
    // .getattr = write_to_drive_getattr,
    .readdir = write_to_drive_readdir,
    .release = write_to_drive_release,
    // //.releasedir = echfs_releasedir,
    .read = write_to_drive_read,
    .write = write_to_drive_write,
    // //.unlink = echfs_unlink,
    // //.utimens = echfs_utimens,
    // //.truncate = echfs_truncate,
    // //.ftruncate = echfs_ftruncate,
    // //.mkdir = echfs_mkdir,
    // //.rmdir = echfs_rmdir,
    // //.rename = echfs_rename,
    .flush = write_to_drive_fflush,
};

int write_to_drive_init() {
    serial_printf("write to drive open pointer %p\n", write_to_drive_open);
    serial_printf("write to drive open pointer2 %p\n", operations.open);
    create_virtual_file(&operations, ENDPOINT_TYPE_DIRECTORY_U, "/lba_drive");
}
