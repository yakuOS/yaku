#pragma once

#include "types.h"

typedef enum {
    TERMINAL_COLOR_BLACK = 0,
    TERMINAL_COLOR_BLUE = 1,
    TERMINAL_COLOR_GREEN = 2,
    TERMINAL_COLOR_CYAN = 3,
    TERMINAL_COLOR_RED = 4,
    TERMINAL_COLOR_MAGENTA = 5,
    TERMINAL_COLOR_BROWN = 6,
    TERMINAL_COLOR_LIGHT_GREY = 7,
    TERMINAL_COLOR_DARK_GREY = 8,
    TERMINAL_COLOR_LIGHT_BLUE = 9,
    TERMINAL_COLOR_LIGHT_GREEN = 10,
    TERMINAL_COLOR_LIGHT_CYAN = 11,
    TERMINAL_COLOR_LIGHT_RED = 12,
    TERMINAL_COLOR_LIGHT_MAGENTA = 13,
    TERMINAL_COLOR_LIGHT_BROWN = 14,
    TERMINAL_COLOR_WHITE = 15,
} terminal_color_t;

void terminal_init(size_t width, size_t height);
void terminal_clear(void);
void terminal_set_style(terminal_color_t fg, terminal_color_t bg);
void terminal_putchar_at(char character, size_t row, size_t column);
void terminal_putchar(char character);
void terminal_puts(const char* string);
