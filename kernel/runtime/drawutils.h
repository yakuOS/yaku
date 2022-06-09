#pragma once

#include <types.h>

#define RGB(r, g, b) ((0 << 24) | ((r) << 16) | ((g) << 8) | (b))

typedef struct {
    void* buffer;
    size_t width, height;
} framebuffer_t;

void drawutils_draw_bordered_rect(framebuffer_t buffer, int x, int y, int width,
                                  int height, uint32_t color_outer_border,
                                  uint32_t color_inner_border, uint32_t color_content);

void drawutils_draw_bordered_rect_default(framebuffer_t buffer, int x, int y, int width,
                                          int height);

void drawutils_draw_line(framebuffer_t buffer, int x1, int y1, int x2, int y2,
                         uint32_t color);

void drawutils_draw_rect_filled(framebuffer_t buffer, size_t x, size_t y, size_t width,
                                size_t height, uint32_t color);

void drawutils_draw_rect(framebuffer_t buffer, size_t x, size_t y, size_t width,
                         size_t height, size_t thickness, uint32_t color);

void drawutils_draw_image_rgba(framebuffer_t buffer, size_t x, size_t y, size_t width,
                               size_t height, const uint32_t* image);

void drawutils_draw_char(framebuffer_t buffer, size_t x, size_t y, char c,
                         uint8_t scaling, uint32_t color);

void drawutils_draw_string(framebuffer_t buffer, size_t x, size_t y, char* str,
                           uint8_t scaling, uint32_t color);

void drawutils_draw_pixel(framebuffer_t buffer, size_t x, size_t y, uint32_t color);
