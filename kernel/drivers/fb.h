#pragma once

#include <types.h>

typedef struct {
    uint32_t width, height, pitch;
    uint8_t bpp;
    void* address;
} fb_info_t;

void fb_init(uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp, void* address);
void fb_set_pixel(uint32_t x, uint32_t y, uint32_t color);
