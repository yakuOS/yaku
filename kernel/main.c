#include <printf.h>

#include "multiboot.h"
#include "terminal.h"
#include "types.h"

void kmain(multiboot_info_t* mb_info) {
    terminal_init(mb_info->framebuffer_width, mb_info->framebuffer_height);
    terminal_set_style(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK);
    char buffer[20];
    sprintf(buffer, "Hello, %s!", "there");
    terminal_puts(buffer);
};
