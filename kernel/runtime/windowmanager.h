#pragma once

#include <types.h>

typedef struct {
    int x, y;
    size_t width, height, index;
} window_t;

void windowmanager_init(void);
void windowmanager_run(void);
void windowmanager_draw(void);
void windowmanager_draw_window(window_t* window);
window_t* windowmanager_get_window_at(size_t x, size_t y);
void windowmanager_draw_rect_filled(size_t x, size_t y, size_t width, size_t height,
                                    uint32_t color);
void windowmanager_draw_rect(size_t x, size_t y, size_t width, size_t height,
                             size_t thickness, uint32_t color);
void windowmanager_draw_pixel(size_t x, size_t y, uint32_t color);
window_t* windowmanager_create_window(size_t width, size_t height);
