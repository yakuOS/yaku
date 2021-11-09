#include "fb.h"

#include <drivers/serial.h>

static fb_info_t fb_info;

void fb_init(uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp,
             void* address) {
    fb_info.width = width;
    fb_info.height = height;
    fb_info.pitch = pitch;
    fb_info.bpp = bpp;
    fb_info.address = address;

    serial_printf("fb_init: %d x %d, bpp: %d, pitch: %d, addr: %p\n", width, height, bpp,
                  pitch, address);
}

void fb_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    switch (fb_info.bpp) {
    case 8:
        *((uint8_t*)fb_info.address + y * fb_info.pitch + x) = color;
        break;

    case 16:
        *((uint16_t*)fb_info.address + y * fb_info.pitch + x) = color;
        break;

    case 32:
        *((uint32_t*)fb_info.address + y * fb_info.pitch + x) = color;
        break;

    default:
        serial_printf("fb_set_pixel: unsupported bpp %d\n", fb_info.bpp);
        break;
    }
}
