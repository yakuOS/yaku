#include <printf.h>

#include <drivers/fb.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <multiboot.h>
#include <types.h>

void kernel_main(multiboot_info_t* mb_info) {
    serial_init();

    if (mb_info->framebuffer_type == MULTIBOOT_FB_TYPE_TEXT) {
        vga_text_init(mb_info->framebuffer_width, mb_info->framebuffer_height);
        vga_text_set_style(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);
    } else if (mb_info->framebuffer_type == MULTIBOOT_FB_TYPE_RGB) {
        fb_init(mb_info->framebuffer_width, mb_info->framebuffer_height,
                mb_info->framebuffer_pitch, mb_info->framebuffer_bpp,
                (void*)mb_info->framebuffer_addr);
    } else {
        serial_printf("kernel_main: unsupported framebuffer type: %d\n",
                      mb_info->framebuffer_type);
    }

    for (uint32_t x = 0; x < mb_info->framebuffer_width; x++) {
        for (uint32_t y = 0; y < mb_info->framebuffer_height; y++) {
            fb_set_pixel(x, y, 0xFFFFFFFF);
        }
    }

    pic_init();
    idt_init();
    pit_init(60);
    ps2_init();

    serial_printf("Hello, %s!\n", "there");

    for (;;) {
        asm("hlt");
    }
};
