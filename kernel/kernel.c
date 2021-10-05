#include <printf.h>

#include <drivers/vga_text.h>
#include <drivers/pit.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <multiboot.h>
#include <types.h>

void kernel_main(multiboot_info_t* mb_info) {
    vga_text_init(mb_info->framebuffer_width, mb_info->framebuffer_height);

    vga_text_set_style(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);

    pic_init();
    idt_init();
    pit_init(60);

    char buffer[20];
    snprintf(buffer, 20, "Hello, %s!\n", "there");
    vga_text_puts(buffer);
    
    for (;;) {
        asm("hlt");
    }
};
