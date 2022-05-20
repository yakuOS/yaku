#include "fb.h"

#include <drivers/serial.h>
#include <drivers/timer.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>

static stivale2_struct_tag_framebuffer_t* fb_tag;

void fb_init(stivale2_struct_tag_framebuffer_t* fb_tag_) {
    fb_tag = fb_tag_;
    serial_printf("fb_init: %d x %d, %d bpp\n", fb_tag->framebuffer_width,
                  fb_tag->framebuffer_height, fb_tag->framebuffer_bpp);
}

void fb_draw_pixel(int x, int y, uint32_t color) {
    // TODO: bounds check and handle different bpp, etc.
    uint8_t* fb = (uint8_t*)fb_tag->framebuffer_addr;
    fb += (y * fb_tag->framebuffer_pitch) + (x * fb_tag->framebuffer_bpp / 8);
    *(uint32_t*)fb = color;
}

void fb_draw_buffer(void* buffer) {
    for (size_t y = 0; y < fb_tag->framebuffer_height; y++) {
        for (size_t x = 0; x < fb_tag->framebuffer_width; x++) {
            fb_draw_pixel(x, y, *(uint32_t*)buffer);
            buffer += 4;
        }
    }
}

uint16_t fb_get_width() {
    return fb_tag->framebuffer_width;
}

uint16_t fb_get_height() {
    return fb_tag->framebuffer_height;
}
