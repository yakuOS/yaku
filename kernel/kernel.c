#include <drivers/fb.h>
#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/lba/lba.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
// #include <echfs/echfs-utils.h>
#include <lib/fuse.h>
// #include <echfs/mkfs.echfs.h>
// #include <echfs/echfs-fuse.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <lib/datetime.h>
#include <lib/file.h>
#include <lib/input/keyboard_handler.h>
#include <lib/input/mouse_handler.h>
#include <lib/write_to_drive.h>
#include <memory/pmm.h>
#include <multitasking/task.h>
#include <printf.h>
#include <resources/keyboard_keymap.h>
#include <runtime/runtime.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>
// #include <drivers/lba/lba.h>
#include <echfs/echfs-fuse.h>
#include <echfs/echfs-utils.h>
#include <lib/syscall_wrapper/open.h>
#include <lib/write_to_drive.h>
#include <virtual_fs/virtual_fs.h>
#include <echfs/mkfs.echfs.h>
#include <lib/stdio.h>
#include <lib/syscall_wrapper/mknod.h>

extern int enable_sse();

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag =
        {
            .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
            .next = 0,
        },
    .flags = 0,
};

static stivale2_header_tag_framebuffer_t framebuffer_hdr_tag = {
    .tag =
        {
            .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
            .next = (uintptr_t)&terminal_hdr_tag,
        },
    // pick best automatically
    .framebuffer_width = 1280,
    .framebuffer_height = 720,
    .framebuffer_bpp = 0,
};

__attribute__((section(".stivale2hdr"), used)) static stivale2_header_t stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};

void* stivale2_get_tag(stivale2_struct_t* stivale2_struct, uint64_t id) {
    stivale2_tag_t* current_tag = (void*)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }
        if (current_tag->identifier == id) {
            return current_tag;
        }
        current_tag = (void*)current_tag->next;
    }
}

void test_task() {
    serial_printf("test task\n");
}
void test_task2() {
    scheduler_sleep(1000);
    serial_printf("hello world\n");
}
int fill_dir(void* dh_, const char* name, const struct stat* statp, off_t off,
             enum fuse_fill_dir_flags flags) {
    serial_printf("dir: %s\n", name);
    return 0;
}
void kernel_main_task() {
    write_to_drive_init();
    // char* argv[4] = {"echfs", "/lba_drive/first_drive", "512", "1"};
    // echfs_mkfs_main(4, argv);

    char* argv2[4] = {"echfs", "", "/lba_drive/first_drive", "/echfsa"};
    echfs_fuse_main(4, argv2);
    serial_printf("echfs fuse main done\n");

    struct dir_entries entries[100];
    // get_dir_entries("/", entries, 100);
    // serial_printf("dir entries: %s, dir entry type %d\n", entries[1].name, entries[0].is_dir);

    mknod("/echfsa/test", S_IFREG, 0);
    get_dir_entries("/echfsa/", entries, 100);
    serial_printf("dir entries: %s, dir entry type %d\n", entries[0].name, entries[0].is_dir);
    char* name = malloc(100);
    name[0] = '/';
    name[1] = 'e';
    name[2] = 'c';
    name[3] = 'h';
    name[4] = 'f';
    name[5] = 's';
    name[6] = 'a';
    name[7] = '/';
    name[8] = 't';
    name[9] = 'e';
    name[10] = 's';
    name[11] = 't';
    FILE* file_des = fopen(name, "r");
    // fwrite("hello world", 1, 11, file_des);
    // fseek(file_des, 0, SEEK_SET);
    // char buf[100];
    // fread(buf, 1, 100, file_des);
    // serial_printf("file contents: %s", buf);

}
void start(stivale2_struct_t* stivale2_struct) {
    enable_sse();
    serial_init();
    pic_init();
    idt_init();
    pit_init(500);

    stivale2_struct_tag_memmap_t* memory_map;
    memory_map = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    pmm_init(memory_map);
    lba_init();
    virtual_fs_init();
    task_add(kernel_main_task, 0, 2, 0);

    // char* args[5] = {"", "", "format", "512"};
    // echfs_utils_main(4, args);

    // virtual_fs_create_directory("/hallo");
    // virtual_fs_create_endpoint(NULL, ENDPOINT_TYPE_FILE, "/hallo/b");
    // asm("cli");
    // ps2_init();
    // input_device_create_device("keyboard", "keyboard", keyboard_keymap,
    //                            &keyboard_handler);
    // input_device_create_device("mouse", "mouse", NULL, &mouse_handler);
    // asm("sti");
    // // task_add(&test_task, TASK_PRIORITY_MEDIUM, 0);
    // // task_add(&test_task2, TASK_PRIORITY_MEDIUM, 0);
    // lba_init();
    // // lba_read_primary_controller(2, 1);
    // // lba_write_primary_controller(2, 1);
    // // lba_read_primary_controller(2, 1);
    // uint8_t buffer[512];
    // buffer[0] = 0b11111111;
    // void* adr = malloc(5000);
    // ((uint8_t*)adr)[0] = 0b11111111;
    // free(adr);
    // // for (int i = 0; i < 256; i++) {
    // //     buffer[i] = 0;
    // // }
    // // serial_printf("timestamp: %lu\n", datetime_get_timestamp());
    // // FILE* file=fopen("/test.txt", "w");
    // // fopen("/test.txt", "w");
    // // fwrite(buffer, 512, 1, file);
    // // uint8_t buffer2[512];
    // // fseek(file, 0, SEEK_SET);
    // // fread(buffer2, 512, 1, file);
    // // buffer[0]=1;
    // // lba_init();
    // // lba_write_primary_controller_first_drive(0, 1, (uint8_t*)buffer);
    // // buffer[0]=0;
    // // lba_write_primary_controller(0, 1, &buffer[0]);

    // // static struct drive_image* image;

    // // image = write_to_drive_fopen(drive_first, W);
    // // serial_printf("%p\n", image);
    // // fseek(image, 0, SEEK_SET);
    // // fwrite(buffer, 1, 10, image);
    // // lba_read_primary_controller_first_drive(0, 1, (uint8_t*)buffer);

    // // fread(buffer, 1, 1, image);
    // // fopen("/b.txt", "w");
    // // fwrite(buffer, 11, 1, fopen("/b.txt", "w"));

    // // char* args[5] = {"-v", "", "import", "/b.txt","r.a"};
    // // echfs_utils_main(5, args);
    // // char* args[4] = {"-v", "", "ls", "/"};
    // // echfs_utils_main(4, args);
    // char* argv[4] = {"echfs", "", "512", "1"};
    // echfs_mkfs_main(4, argv);
    // malloc(1000*1000);
    // struct fuse_operations fuse_ops;
    // echfs_get_fuse_operations(&fuse_ops);
    // fuse_ops.init(NULL);
    // fuse_fill_dir_t ffill_dir = {0};
    // struct fuse_file_info file_info = {0};
    // fuse_ops.opendir("/", &file_info);

    // fuse_ops.mkdir("/test", 0 | S_IFDIR);
    // fuse_ops.mkdir("/jo", 0 | S_IFDIR);
    // fuse_ops.create("/test.txt", 0 | S_IFREG, &file_info);

    // fuse_ops.opendir("/", &file_info);
    // fuse_ops.readdir("/", 0, fill_dir,0,&file_info);
    // fuse_ops.init(NULL);
    // echfs_fuse_main(NULL, NULL);
    // FILE* file=fopen("/test.txt", "w");
    for (;;) {
        asm("hlt");
    }
};
