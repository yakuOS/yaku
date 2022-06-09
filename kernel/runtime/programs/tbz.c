#include "tbz.h"

#include <resources/tbz_logo.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>

void tbz_main() {
    window_t* window = windowmanager_create_window(350, 150, "TBZ");

    drawutils_draw_rect_filled(window->buffer, 0, 0, 350, 150, RGB(255, 255, 255));

    drawutils_draw_image_rgba(window->buffer, 25, 12, TBZ_LOGO_WIDTH, TBZ_LOGO_HEIGHT,
                              (const uint32_t*)tbz_logo);
}
