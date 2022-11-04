#pragma once
#include <lib/origin.h>
#include <types.h>
enum drive { drive_first, drive_second, drive_third, drive_fourth };
enum access_mode { R, W }; // Read, Write
struct drive_image {
    enum access_mode access_mode;
    enum drive drive;
    bool occupied;
};


int write_to_drive_init();
