#pragma once

#include <types.h>

#define VGA_TEXT_ENTRY(character, style) ((uint16_t)character | (uint16_t)style << 8)

typedef enum {
    VGA_TEXT_COLOR_BLACK = 0,
    VGA_TEXT_COLOR_BLUE = 1,
    VGA_TEXT_COLOR_GREEN = 2,
    VGA_TEXT_COLOR_CYAN = 3,
    VGA_TEXT_COLOR_RED = 4,
    VGA_TEXT_COLOR_MAGENTA = 5,
    VGA_TEXT_COLOR_BROWN = 6,
    VGA_TEXT_COLOR_LIGHT_GREY = 7,
    VGA_TEXT_COLOR_DARK_GREY = 8,
    VGA_TEXT_COLOR_LIGHT_BLUE = 9,
    VGA_TEXT_COLOR_LIGHT_GREEN = 10,
    VGA_TEXT_COLOR_LIGHT_CYAN = 11,
    VGA_TEXT_COLOR_LIGHT_RED = 12,
    VGA_TEXT_COLOR_LIGHT_MAGENTA = 13,
    VGA_TEXT_COLOR_LIGHT_BROWN = 14,
    VGA_TEXT_COLOR_WHITE = 15,
} vga_text_color_t;

void vga_text_init(size_t width, size_t height);
void vga_text_clear(void);
void vga_text_set_style(vga_text_color_t fg, vga_text_color_t bg);
void vga_text_putchar_at(char character, size_t row, size_t column);
void vga_text_putchar(char character);
void vga_text_puts(const char* string);
