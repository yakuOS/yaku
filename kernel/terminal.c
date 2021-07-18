#include "terminal.h"

#include "types.h"

#define VGA_ENTRY(character, style) ((uint16_t)character | (uint16_t)style << 8)

typedef struct {
    size_t width, height, row, column;
    uint8_t style;
} terminal_info_t;

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static terminal_info_t term_info;

void terminal_init(size_t width, size_t height) {
    term_info.width = width;
    term_info.height = height;
    term_info.row = 0;
    term_info.column = 0;

    terminal_clear();
    terminal_set_style(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
}

void terminal_clear(void) {
    uint16_t blank_entry = VGA_ENTRY(' ', 15);

    for (size_t i = 0; i < term_info.height * term_info.width; i++) {
        VGA_MEMORY[i] = blank_entry;
    }

    term_info.row = 0;
    term_info.column = 0;
}

void terminal_set_style(terminal_color_t fg, terminal_color_t bg) {
    term_info.style = (uint8_t)bg << 4 | (uint8_t)fg;
}

void terminal_putchar_at(char character, size_t row, size_t column) {
    VGA_MEMORY[row * term_info.width + column] = VGA_ENTRY(character, term_info.style);
}

void terminal_putchar(char character) {
    if (character == '\n') {
        term_info.row++;
        term_info.column = 0;
        return;
    }

    terminal_putchar_at(character, term_info.row, term_info.column);

    if (++term_info.column == term_info.width) {
        term_info.column = 0;
        if (++term_info.row == term_info.height) {
            // scrolling will be implemented later
            // for now, just clear the terminal and begin at 0,0
            terminal_clear();
        }
    }
}

void terminal_puts(const char* string) {
    while (*string != 0) {
        terminal_putchar(*string++);
    }
}
