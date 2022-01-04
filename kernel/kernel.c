#include <printf.h>

#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <resources/keyboard_keymap.h>
#include <multiboot.h>
#include <types.h>

void kernel_main(multiboot_info_t* mb_info) {
    serial_init();

    vga_text_init(mb_info->framebuffer_width, mb_info->framebuffer_height);
    vga_text_set_style(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);

    pic_init();
    idt_init();
    pit_init(60);
    ps2_init();
    input_device_create_device("keyboard", "keyboard", keyboard_keymap);

    serial_printf("Hello, %s!\n", "there");

    for (;;) {
        asm("hlt");
    }
};
