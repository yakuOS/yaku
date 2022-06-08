#include "tbz.h"

#include <resources/tbz_logo.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>

void tbz_main() {
    window_t* window = windowmanager_create_window(350, 150);

    drawutils_draw_rect_filled(window->buffer, 0, 0, 350, 150, RGB(255, 255, 255));

    for (size_t x = 0; x < TBZ_LOGO_WIDTH; x++) {
        for (size_t y = 0; y < TBZ_LOGO_HEIGHT; y++) {
            uint32_t color = ((uint32_t*)tbz_logo)[(y * TBZ_LOGO_WIDTH) + x];

            uint8_t r = color & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 16) & 0xFF;

            drawutils_draw_pixel(window->buffer, x + 25, y + 12, RGB(r, g, b));
        }
    }
}
