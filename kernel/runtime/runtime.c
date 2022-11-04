#include "runtime.h"

#include <multitasking/task.h>
#include <runtime/windowmanager.h>
#include <echfs/echfs-fuse.h>
#include <echfs/mkfs.echfs.h>
#include <drivers/serial.h>
#include <lib/stdio.h>
#include <lib/syscall_wrapper/mknod.h>
#include <lib/syscall_wrapper/remove_virtual_file.h>


// programs (will be read from hard drive later)
#include <runtime/programs/cube.h>
#include <runtime/programs/editor.h>
#include <runtime/programs/gradient.h>
#include <runtime/programs/tbz.h>

#include <lib/syscall_wrapper/get_open_pointer.h>

void runtime_start() {

    write_to_drive_init();
    serial_printf("write to drive init done\n");
    get_open_pointer();
    serial_printf("runtime_start\n");
    char** argv_fuse_mkfs[4] = {"echfs", "/lba_drive/first_drive", "512", "1"};
    serial_printf("a %c\n", argv_fuse_mkfs[1]);
    echfs_mkfs_main(4, argv_fuse_mkfs);
    serial_printf("b\n");
    char** argv2[4] = {"echfs", "", "/lba_drive/first_drive", "/echfsa"};
    echfs_fuse_main(4, argv2);
    serial_printf("echfs fuse main done\n");
    windowmanager_init();
    task_add(&windowmanager_run, NULL, TASK_PRIORITY_VERY_HIGH, 0);

    // program startup
    // task_add(&cube_main, TASK_PRIORITY_MEDIUM, 0);
    task_add(&gradient_main, NULL, TASK_PRIORITY_LOW, 0);
    task_add(&tbz_main, NULL, TASK_PRIORITY_LOW, 0);
    task_add(&editor_main, NULL, TASK_PRIORITY_LOW, 0);



    mknod("/echfsa/test", S_IFREG, 0);
    FILE* file_des = fopen("echfsa/test/", "r");
    fputs("hello world", file_des);
    fseek(file_des, 0, SEEK_SET);
    char buf[100];
    fgets(buf, 20, file_des);
    serial_printf("file contents: %s \n", buf);
    fclose(file_des);
    remove_virtual_file("/echfsa");
}
