#include "write_to_drive.h"
#include <drivers/lba/lba.h>
#include <drivers/serial.h>
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

int write_to_drive_fopen(const char* drive, struct fuse_file_info* fh) {
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


// uint8_t write_to_drive_fseek(struct drive_image* drive_image, int64_t offset /*in bytes*/,
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
//             if (get_drive_size(secondary_controller, second_drive) * 512 + offset < 0) {
//                 return 0;
//             }
//             drive_image->byte_pointer_position =
//                 get_drive_size(secondary_controller, second_drive) * 512 + offset;
//             return 1;
//         }
//     }
//     return 0;
// }

off_t write_to_drive_fwrite(const char* path, const char* buf, size_t size, off_t offset,
                              struct fuse_file_info* fh) {
    struct drive_image* image = &drive_images[fh->fh];
    if (image->access_mode != W) {
        return 0;
    }
    uint64_t byte_to_write_to = (uint64_t)offset;
    uint64_t sector_to_write_to = byte_to_write_to / 512;
    uint64_t byte_in_sector_to_write_to = byte_to_write_to % 512;

    uint8_t* ptr = (uint8_t*)buf;

    if (size % 512 == 0) { // if the size of the element is a multiple of
                                      // 512 just write the buffer to the drive
        if (image->drive == drive_first) {

            lba_write_primary_controller_first_drive(
                sector_to_write_to, size / 512, ptr);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, size / 512, ptr);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, size / 512, ptr);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, size / 512, ptr);
        }
    } else { // else read the sector from the drive, add the buffer to it and write it
             // back to the drive
        uint8_t buffer[(size) +
                       (512 - ((size) %
                               512))]; // buffer with size of element*number_of_elements
                                       // rounded up to 512 (sector size)
        if (image->drive == drive_first) {
            lba_read_primary_controller_first_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer); // size / 512+1->
                                   // because size % 512
                                   // != 0, we need to read one sector more than
                                   // size
        } else if (image->drive == drive_second) {
            lba_read_primary_controller_second_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_read_secondary_controller_first_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_read_secondary_controller_second_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        }
        for (uint64_t i = byte_in_sector_to_write_to;
             i < byte_in_sector_to_write_to + size; i++) {
            buffer[i] = ptr[i - byte_in_sector_to_write_to];
        }
        if (image->drive == drive_first) {
            lba_write_primary_controller_first_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, size / 512 + 1,
                (uint8_t*)buffer);
        }
        image->byte_pointer_position += size;
    }
    return 1;
}

uint8_t write_to_drive_fputs(char* str, struct drive_image* image) {
    uint64_t size_of_element = strlen(str);
    uint8_t string_buffer[size_of_element];
    strncpy((char*)string_buffer, str, size_of_element);
    if (image->access_mode != W) {
        return 0;
    }
    uint64_t byte_to_write_to = image->byte_pointer_position;
    uint64_t sector_to_write_to = byte_to_write_to / 512;
    uint64_t byte_in_sector_to_write_to = byte_to_write_to % 512;
    if (size_of_element % 512 == 0) { // if the size of the element is a multiple of
                                      // 512 just write the buffer to the drive
        if (image->drive == drive_first) {
            lba_write_primary_controller_first_drive(
                sector_to_write_to, size_of_element / 512, (uint8_t*)string_buffer);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, size_of_element / 512, (uint8_t*)string_buffer);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, size_of_element / 512, (uint8_t*)string_buffer);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, size_of_element / 512, (uint8_t*)string_buffer);
        }
        image->byte_pointer_position += size_of_element;
    } else { // else read the sector from the drive, add the buffer to it and write it
             // back to the drive
        uint8_t buffer[(size_of_element) +
                       (512 - ((size_of_element) %
                               512))]; // buffer with size of element*number_of_elements
                                       // rounded up to 512 (sector size)
        if (image->drive == drive_first) {
            lba_read_primary_controller_first_drive(
                sector_to_write_to, size_of_element / 512 + 1,
                (uint8_t*)buffer); // size / 512+1->
                                   // because size % 512
                                   // != 0, we need to read one sector more than
                                   // size
        } else if (image->drive == drive_second) {
            lba_read_primary_controller_second_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_read_secondary_controller_first_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_read_secondary_controller_second_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        }
        for (uint64_t i = byte_in_sector_to_write_to;
             i < byte_in_sector_to_write_to + size_of_element; i++) {
            buffer[i] = string_buffer[i - byte_in_sector_to_write_to];
        }
        if (image->drive == drive_first) {
            lba_write_primary_controller_first_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, size_of_element / 512 + 1, (uint8_t*)buffer);
        }
        image->byte_pointer_position += size_of_element;
    }
    return 1;
}

uint8_t write_to_drive_fflush(struct drive_image* image) {
    return 1;
}

uint8_t write_to_drive_fgetc(struct drive_image* image) {
    uint64_t byte_to_read_from = image->byte_pointer_position;
    uint64_t sector_to_read_from = byte_to_read_from / 512;
    uint64_t byte_in_sector_to_read_from = byte_to_read_from % 512;
    uint8_t buffer[512];
    if (image->drive == drive_first) {
        lba_read_primary_controller_first_drive(sector_to_read_from, 1, (uint8_t*)buffer);
    } else if (image->drive == drive_second) {
        lba_read_primary_controller_second_drive(sector_to_read_from, 1,
                                                 (uint8_t*)buffer);
    } else if (image->drive == drive_third) {
        lba_read_secondary_controller_first_drive(sector_to_read_from, 1,
                                                  (uint8_t*)buffer);
    } else if (image->drive == drive_fourth) {
        lba_read_secondary_controller_second_drive(sector_to_read_from, 1,
                                                   (uint8_t*)buffer);
    }
    image->byte_pointer_position++;
    return buffer[byte_in_sector_to_read_from];
}

size_t write_to_drive_fread(uint8_t* ptr, size_t size_of_element,
                            size_t number_of_elements, struct drive_image* image) {

    uint64_t byte_to_read_from = image->byte_pointer_position;
    uint64_t sector_to_read_from = byte_to_read_from / 512;
    uint64_t byte_in_sector_to_read_from = byte_to_read_from % 512;
    uint8_t* buffer /*[(size_of_element * number_of_elements) +
                    (512 - ((size_of_element * number_of_elements) %
                            512))]*/
        = (uint8_t*)malloc(
            (size_of_element * number_of_elements) +
            (512 - ((size_of_element * number_of_elements) %
                    512))); // buffer with size of element*number_of_elements
                            // rounded up to 512 (sector size)
    uint64_t sectors_to_read = 0;
    if (size_of_element % 512 == 0) {
        sectors_to_read = number_of_elements * size_of_element / 512;
    } else {
        sectors_to_read = number_of_elements * size_of_element / 512 + 1;
    }
    if (image->drive == drive_first) {
        lba_read_primary_controller_first_drive(sector_to_read_from, sectors_to_read,
                                                (uint8_t*)buffer);
    } else if (image->drive == drive_second) {
        lba_read_primary_controller_second_drive(sector_to_read_from, sectors_to_read,
                                                 (uint8_t*)buffer);
    } else if (image->drive == drive_third) {
        lba_read_secondary_controller_first_drive(sector_to_read_from, sectors_to_read,
                                                  (uint8_t*)buffer);
    } else if (image->drive == drive_fourth) {
        lba_read_secondary_controller_second_drive(sector_to_read_from, sectors_to_read,
                                                   (uint8_t*)buffer);
    }

    // copy the data from where to read from to the buffer
    for (uint64_t i = byte_in_sector_to_read_from;
         i < byte_in_sector_to_read_from + size_of_element * number_of_elements; i++) {
        ptr[i - byte_in_sector_to_read_from] = buffer[i];
    }
    image->byte_pointer_position += size_of_element * number_of_elements;
    free(buffer);
    return number_of_elements;
}

static struct fuse_operations operations = {
    .open = write_to_drive_fopen,
    //.opendir = echfs_opendir,
    //.fgetattr = echfs_fgetattr,
    //.getattr = echfs_getattr,
    //.readdir = echfs_readdir,
    // .release = write_to_drive_release,
    // //.releasedir = echfs_releasedir,
    // //.read = echfs_read,
    // .write = write_to_drive_fwrite,
    // //.create = echfs_create,
    // //.unlink = echfs_unlink,
    // //.utimens = echfs_utimens,
    // //.truncate = echfs_truncate,
    // //.ftruncate = echfs_ftruncate,
    // //.mkdir = echfs_mkdir,
    // //.rmdir = echfs_rmdir,
    // //.rename = echfs_rename,
    // .flush = write_to_drive_fflush,
};

int write_to_drive_init() {
    create_virtual_file(&operations, ENDPOINT_TYPE_FILE_U, "/lba_drive");
}
