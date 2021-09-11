#include "vga_text.h"

typedef struct {
    size_t width, height, row, column;
    uint8_t style;
} vga_text_info_t;

static uint16_t* const VGA_TEXT_MEMORY = (uint16_t*)0xB8000;

static vga_text_info_t term_info;

void vga_text_init(size_t width, size_t height) {
    term_info.width = width;
    term_info.height = height;
    term_info.row = 0;
    term_info.column = 0;

    vga_text_clear();
    vga_text_set_style(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLACK);
}

void vga_text_clear(void) {
    uint16_t blank_entry = VGA_TEXT_ENTRY(' ', 15);

    for (size_t i = 0; i < term_info.height * term_info.width; i++) {
        VGA_TEXT_MEMORY[i] = blank_entry;
    }

    term_info.row = 0;
    term_info.column = 0;
}

void vga_text_set_style(vga_text_color_t fg, vga_text_color_t bg) {
    term_info.style = (uint8_t)bg << 4 | (uint8_t)fg;
}

void vga_text_putchar_at(char character, size_t row, size_t column) {
    VGA_TEXT_MEMORY[row * term_info.width + column] =
        VGA_TEXT_ENTRY(character, term_info.style);
}

void vga_text_putchar(char character) {
    if (character == '\n') {
        term_info.row++;
        term_info.column = 0;
        return;
    }

    vga_text_putchar_at(character, term_info.row, term_info.column);

    if (++term_info.column == term_info.width) {
        term_info.column = 0;
        if (++term_info.row == term_info.height) {
            // scrolling will be implemented later
            // for now, just clear the screen and begin at 0,0
            vga_text_clear();
        }
    }
}

void vga_text_puts(const char* string) {
    while (*string != 0) {
        vga_text_putchar(*string++);
    }
}
