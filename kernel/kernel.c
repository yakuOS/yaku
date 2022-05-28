#include <drivers/fb.h>
#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <lib/input/keyboard_handler.h>
#include <lib/input/mouse_handler.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <printf.h>
#include <resources/keyboard_keymap.h>
#include <runtime/windowmanager.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>

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

void start(stivale2_struct_t* stivale2_struct) {
    enable_sse();
    serial_init();
    pic_init();
    idt_init();
    pit_init(1000);

    stivale2_struct_tag_memmap_t* memory_map;
    memory_map = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    pmm_init(memory_map);

    asm("cli");
    ps2_init();
    input_device_create_device("keyboard", "keyboard", keyboard_keymap,
                               &keyboard_handler);
    input_device_create_device("mouse", "mouse", NULL, &mouse_handler);
    asm("cli");

    stivale2_struct_tag_framebuffer_t* fb_tag;
    fb_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    fb_init(fb_tag);

    windowmanager_init();
    windowmanager_create_window(500, 500);
    windowmanager_create_window(200, 300);
    windowmanager_run();

    for (;;) {
        asm("hlt");
    }
};
