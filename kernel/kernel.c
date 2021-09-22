#include <printf.h>

#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <multiboot.h>
#include <types.h>

void kernel_main(multiboot_info_t* mb_info) {
    pic_disable();
    idt_init();
    vga_text_init(mb_info->framebuffer_width, mb_info->framebuffer_height);

    vga_text_set_style(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);

    char buffer[20];
    sprintf(buffer, "Hello, %s!", "there");
    vga_text_puts(buffer);
    
    while(1) __asm__("hlt");
};
