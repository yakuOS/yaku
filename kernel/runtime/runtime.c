#include "runtime.h"

#include <drivers/serial.h>
#include <echfs/echfs-fuse.h>
#include <echfs/mkfs.echfs.h>
#include <lib/stdio.h>
#include <lib/syscall_wrapper/mknod.h>
#include <lib/syscall_wrapper/remove_virtual_file.h>
#include <multitasking/task.h>
#include <runtime/windowmanager.h>

#include <interrupts/pic.h>
// programs (will be read from hard drive later)
#include <runtime/programs/cube.h>
#include <runtime/programs/editor.h>
#include <runtime/programs/gradient.h>
#include <runtime/programs/tbz.h>

#include <lib/syscall_wrapper/get_open_pointer.h>

extern int enable_sse();
extern int test_sse();
struct entry_t {
    uint64_t parent_id;
    uint8_t type;
    char name[100];
    uint64_t atime;
    uint64_t mtime;
    uint16_t perms;
    uint16_t owner;
    uint16_t group;
    uint64_t ctime;
    uint64_t payload;
    uint64_t size;
} __attribute__((packed));

struct path_result_t {
    uint64_t target_entry;
    struct entry_t target;
    struct entry_t parent;
    char name[100];
    char path[1000];
    int failure;
    int not_found;
    uint8_t type;
    struct path_result_t* next;
};
struct path_result_table {
    struct path_result_t** table;
    uint64_t size;
    uint64_t num_elements;
};
void runtime_start() {
    // test_sse();
    serial_printf("Starting runtime\n");
    // asm("cli");
    // struct path_result_table table = {0};
    // asm("sti");
    // serial_printf("Starting runtime2\n");
    // pic_mask_irq(0);
    write_to_drive_init();
    serial_printf("write to drive init done\n");
    // get_open_pointer();
    // serial_printf("runtime_start\n");
    char* argv_fuse_mkfs[4] = {"echfs", "/lba_drive/first_drive", "512", "1"};
    // serial_printf("a %c\n", argv_fuse_mkfs[1]);
    echfs_mkfs_main(4, NULL);
    // serial_printf("b\n");
    // // enable_sse();
    // serial_printf("%lu\n", test_sse());
    serial_printf("testing sse done\n");
    // struct fuse_operations ops = {0};
    // char* argv2[4] = {"echfs", "", "/lba_drive/first_drive", "/echfsa"};
    // // // test_sse();
    char* argv_fuse_mkfs2[] = {"echfs", "/lba_drive/first_drive",
                               "/lba_drive/first_drive", "/echfsa"};
    // // struct path_result_table table = {0};
    serial_printf("c\n");
    echfs_fuse_main(4, argv_fuse_mkfs2);
    // serial_printf("echfs fuse main done\n");
    // windowmanager_init();
    // task_add(&windowmanager_run, NULL, TASK_PRIORITY_VERY_HIGH, 0);

    // program startup
    // task_add(&cube_main, NULL, TASK_PRIORITY_MEDIUM, 0);
    // task_add(&gradient_main, NULL, TASK_PRIORITY_LOW, 0);
    // task_add(&tbz_main, NULL, TASK_PRIORITY_LOW, 0);
    // task_add(&editor_main, NULL, TASK_PRIORITY_LOW, 0);

    mknod("/echfsa/test", S_IFREG, 0);
    // serial_printf("runtime check 4\n");
    FILE* file_des = fopen("/echfsa/test/", "r");
    serial_printf("runtime check 3");
    fputs("hello world", file_des);
    fseek(file_des, 0, SEEK_SET);
    char buf[100];
    fgets(buf, 20, file_des);
    serial_printf("file contents: %s \n", buf);
    // fclose(file_des);
    // remove_virtual_file("/echfsa");
}
