#pragma once

#include <stivale2.h>
#include <types.h>

void fb_init(stivale2_struct_tag_framebuffer_t* fb_tag);
void fb_draw_pixel(int x, int y, uint32_t color);
void fb_draw_buffer(void* buffer);
uint16_t fb_get_width(void);
uint16_t fb_get_height(void);
