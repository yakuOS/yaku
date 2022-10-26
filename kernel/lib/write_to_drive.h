#pragma once
#include <lib/origin.h>
#include <types.h>
enum drive { drive_first, drive_second, drive_third, drive_fourth };
enum access_mode { R, W }; // Read, Write
struct drive_image {
    enum access_mode access_mode;
    enum drive drive;
    uint64_t byte_pointer_position;
};

/*
 * creates a drive_image struct and returns it
 * if the drive_image could not be created, returns NULL
 */
struct drive_image* write_to_drive_fopen(enum drive drive, enum access_mode access_mode);

// sets the current position pointer of the drive_image to the given position
// SEEK_SET: sets the position pointer to the given offset
// SEEK_CUR: sets the position pointer to the current position pointer + the given offset
// SEEK_END: sets the position pointer to the end of the drive_image + the given offset
uint8_t write_to_drive_fseek(struct drive_image* drive_image, int64_t offset,
                             enum origin origin);

// resets the current position pointer of the drive_image to 0
void write_to_drive_rewind(struct drive_image* drive_image);

/*
 * returns the current position pointer of the drive_image
 */
uint64_t write_to_drive_ftell(struct drive_image* drive_image);


// writes the given buffer to the drive_image at the current position pointer
// increase the current position pointer
uint8_t write_to_drive_fwrite(uint8_t* ptr, size_t size_of_element,
                              uint8_t number_of_elements, struct drive_image* image);

// writes a given string to the drive_image at the current position pointer
// increase the current position pointer
uint8_t write_to_drive_fputs(char* str, struct drive_image* image);

// read size_of_element * number_of_elements bytes from the drive_image at the current
// position pointer increase the current position pointer
size_t write_to_drive_fread(uint8_t* ptr, size_t size_of_element,
                             size_t number_of_elements, struct drive_image* image);

// get a char from the drive_image at the current position pointer
// increase the current position pointer
uint8_t write_to_drive_fgetc(struct drive_image* image);

/*
 * frees the memory of the drive_image
 */
void write_to_drive_fclose(struct drive_image* drive_image);

// no functionality right now
uint8_t write_to_drive_fflush(struct drive_image* image);
