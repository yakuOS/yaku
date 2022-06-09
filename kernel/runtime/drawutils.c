#include "drawutils.h"

#include <resources/font8x8.h>
#include <types.h>

void drawutils_draw_bordered_rect(framebuffer_t buffer, int x, int y, int width,
                                  int height, uint32_t color_outer_border,
                                  uint32_t color_inner_border, uint32_t color_content) {
    drawutils_draw_rect(buffer, x, y, width, height, 1, color_outer_border);
    drawutils_draw_rect(buffer, x + 1, y + 1, width - 2, height - 2, 1,
                        color_inner_border);
    drawutils_draw_rect_filled(buffer, x + 2, y + 2, width - 4, height - 4,
                               color_content);
}

void drawutils_draw_bordered_rect_default(framebuffer_t buffer, int x, int y, int width,
                                          int height) {
    drawutils_draw_bordered_rect(buffer, x, y, width, height, RGB(195, 195, 195),
                                 RGB(255, 255, 255), RGB(195, 195, 195));
}

void drawutils_draw_line(framebuffer_t buffer, int x1, int y1, int x2, int y2,
                         uint32_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawutils_draw_pixel(buffer, x1, y1, color);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawutils_draw_rect_filled(framebuffer_t buffer, size_t x, size_t y, size_t width,
                                size_t height, uint32_t color) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            drawutils_draw_pixel(buffer, x + j, y + i, color);
        }
    }
}

void drawutils_draw_rect(framebuffer_t buffer, size_t x, size_t y, size_t width,
                         size_t height, size_t thickness, uint32_t color) {
    for (size_t i = 0; thickness > 0; thickness--, i++) {
        for (size_t j = 0; j < width; j++) {
            drawutils_draw_pixel(buffer, x + j, y + i, color);
            drawutils_draw_pixel(buffer, x + j, y + height - i - 1, color);
        }

        for (size_t j = 0; j < height; j++) {
            drawutils_draw_pixel(buffer, x + i, y + j, color);
            drawutils_draw_pixel(buffer, x + width - i - 1, y + j, color);
        }
    }
}

void drawutils_draw_image_rgba(framebuffer_t buffer, size_t x, size_t y, size_t width,
                               size_t height, const uint32_t* image) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            uint32_t color = image[i * width + j];

            uint8_t r = color & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 16) & 0xFF;
            uint8_t a = (color >> 24) & 0xFF;

            if (a == 0) {
                continue;
            }

            drawutils_draw_pixel(buffer, x + j, y + i, RGB(r, g, b));
        }
    }
}

void drawutils_draw_char(framebuffer_t buffer, size_t x, size_t y, char c,
                         uint8_t scaling, uint32_t color) {
    if (c < 0x20 || c > 0x7F) {
        return;
    }

    const uint8_t* char_bitmap = font8x8[c];

    // scaling 2 means 4 pixels for each pixel in the bitmap
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (char_bitmap[i] & (1 << j)) {
                drawutils_draw_rect_filled(buffer, x + j * scaling, y + i * scaling,
                                           scaling, scaling, color);
            }
        }
    }
}

void drawutils_draw_string(framebuffer_t buffer, size_t x, size_t y, char* str,
                           uint8_t scaling, uint32_t color) {
    size_t old_x = x;

    for (; *str; str++) {
        if (*str == '\n') {
            x = old_x;
            y += 12 * scaling;
        } else {
            drawutils_draw_char(buffer, x, y, *str, scaling, color);
            x += 8 * scaling;
        }
    }
}

void drawutils_draw_pixel(framebuffer_t buffer, size_t x, size_t y, uint32_t color) {
    if (x >= buffer.width || y >= buffer.height) {
        return;
    }

    ((uint32_t*)buffer.buffer)[y * buffer.width + x] = color;
}
