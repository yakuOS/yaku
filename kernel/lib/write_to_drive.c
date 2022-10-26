#include "write_to_drive.h"
#include <drivers/lba/lba.h>
#include <drivers/serial.h>
#include <memory/pmm.h>
#include <thirdparty/string/string.h>

struct drive_image* write_to_drive_fopen(enum drive drive, enum access_mode access_mode) {
    // check if drive is recognized
    if (drive == drive_first) {
        if (!drive_present(primary_controller, first_drive)) {
            return NULL;
        }
    } else if (drive == drive_second) {
        if (!drive_present(primary_controller, second_drive)) {
            return NULL;
        }
    } else if (drive == drive_third) {
        if (!drive_present(secondary_controller, first_drive)) {
            return NULL;
        }
    } else if (drive == drive_fourth) {
        if (!drive_present(secondary_controller, second_drive)) {
            return NULL;
        }
    }
    struct drive_image* drive_image =
        (struct drive_image*)malloc((sizeof(struct drive_image) - 1) / 4069 + 1);
    drive_image->drive = drive;
    drive_image->access_mode = access_mode;
    drive_image->byte_pointer_position = 0;
    return drive_image;
}

uint64_t write_to_drive_ftell(struct drive_image* drive_image) {
    return drive_image->byte_pointer_position;
}

void write_to_drive_fclose(struct drive_image* drive_image) {
    free(drive_image);
}

void write_to_drive_rewind(struct drive_image* drive_image) {
    drive_image->drive = 0;
}

uint8_t write_to_drive_fseek(struct drive_image* drive_image, int64_t offset /*in bytes*/,
                             enum origin origin) {
    if (origin == SEEK_SET) {
        if (offset < 0) {
            return 0;
        }
        drive_image->byte_pointer_position = offset;
        return 1;
    } else if (origin == SEEK_CUR) {
        if (drive_image->byte_pointer_position + offset < 0) {
            return 0;
        }
        drive_image->byte_pointer_position += offset;
        return 1;
    } else if (origin = SEEK_END) {
        if (drive_image->drive == drive_first) {
            // check if drive is present
            if (!drive_present(primary_controller, first_drive)) {
                return 0;
            }
            // check if new pointer would be < 0
            if (get_drive_size(primary_controller, first_drive) * 512 + offset < 0) {
                return 0;
            }
            drive_image->byte_pointer_position =
                get_drive_size(primary_controller, first_drive) * 512 + offset;
            return 1;
        } else if (drive_image->drive == drive_second) {
            if (!drive_present(primary_controller, second_drive)) {
                return 0;
            }
            if (get_drive_size(primary_controller, second_drive) * 512 + offset < 0) {
                return 0;
            }
            drive_image->byte_pointer_position =
                get_drive_size(primary_controller, second_drive) * 512 + offset;
            return 1;
        } else if (drive_image->drive == drive_third) {
            if (!drive_present(secondary_controller, first_drive)) {
                return 0;
            }
            if (get_drive_size(secondary_controller, first_drive) * 512 + offset < 0) {
                return 0;
            }
            drive_image->byte_pointer_position =
                get_drive_size(secondary_controller, first_drive) * 512 + offset;
            return 1;
        } else if (drive_image->drive == drive_fourth) {
            if (!drive_present(secondary_controller, second_drive)) {
                return 0;
            }
            if (get_drive_size(secondary_controller, second_drive) * 512 + offset < 0) {
                return 0;
            }
            drive_image->byte_pointer_position =
                get_drive_size(secondary_controller, second_drive) * 512 + offset;
            return 1;
        }
    }
    return 0;
}

uint8_t write_to_drive_fwrite(uint8_t* ptr, size_t size_of_element,
                              uint8_t number_of_elements, struct drive_image* image) {
    if (image->access_mode != W) {
        return 0;
    }
    uint64_t byte_to_write_to = image->byte_pointer_position;
    uint64_t sector_to_write_to = byte_to_write_to / 512;
    uint64_t byte_in_sector_to_write_to = byte_to_write_to % 512;

    if (size_of_element % 512 == 0) { // if the size of the element is a multiple of 512
                                      // just write the buffer to the drive
        if (image->drive == drive_first) {

            lba_write_primary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512, ptr);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512, ptr);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512, ptr);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512, ptr);
        }
        image->byte_pointer_position += number_of_elements * size_of_element;
    } else { // else read the sector from the drive, add the buffer to it and write it
             // back to the drive
        uint8_t buffer[(size_of_element * number_of_elements) +
                       (512 - ((size_of_element * number_of_elements) %
                               512))]; // buffer with size of element*number_of_elements
                                       // rounded up to 512 (sector size)
        if (image->drive == drive_first) {
            lba_read_primary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer); // number_of_elements * size_of_element / 512+1->
                                   // because number_of_elements * size_of_element % 512
                                   // != 0, we need to read one sector more than
                                   // number_of_elements * size_of_element
        } else if (image->drive == drive_second) {
            lba_read_primary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_read_secondary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_read_secondary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        }
        for (uint64_t i = byte_in_sector_to_write_to;
             i < byte_in_sector_to_write_to + size_of_element * number_of_elements; i++) {
            buffer[i] = ptr[i - byte_in_sector_to_write_to];
        }
        if (image->drive == drive_first) {
            lba_write_primary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_second) {
            lba_write_primary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_third) {
            lba_write_secondary_controller_first_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        } else if (image->drive == drive_fourth) {
            lba_write_secondary_controller_second_drive(
                sector_to_write_to, number_of_elements * size_of_element / 512 + 1,
                (uint8_t*)buffer);
        }
        image->byte_pointer_position += number_of_elements * size_of_element;
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
    if (size_of_element % 512 == 0) { // if the size of the element is a multiple of 512
                                      // just write the buffer to the drive
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
                (uint8_t*)buffer); // number_of_elements * size_of_element / 512+1->
                                   // because number_of_elements * size_of_element % 512
                                   // != 0, we need to read one sector more than
                                   // number_of_elements * size_of_element
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
    uint8_t* buffer/*[(size_of_element * number_of_elements) +
                   (512 - ((size_of_element * number_of_elements) %
                           512))]*/ = (uint8_t*) malloc((size_of_element * number_of_elements) +
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
