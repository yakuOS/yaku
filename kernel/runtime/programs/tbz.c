#include "tbz.h"

#include <drivers/serial.h>
#include <resources/tbz_logo.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>

void tbz_main() {
    window_t* window = windowmanager_create_window(350, 150, "TBZ");
    serial_printf("TBZ main 2\n");
    drawutils_draw_rect_filled(window->buffer, 0, 0, 350, 150, RGB(255, 255, 255));
    serial_printf("TBZ main 3\n");
    drawutils_draw_image_rgba(window->buffer, 25, 12, TBZ_LOGO_WIDTH, TBZ_LOGO_HEIGHT,
                              (const uint32_t*)tbz_logo);
    serial_printf("TBZ main 4\n");
}
