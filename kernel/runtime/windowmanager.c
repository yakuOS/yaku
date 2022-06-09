#include "windowmanager.h"

#include <drivers/fb.h>
#include <drivers/serial.h>
#include <lib/input/mouse_handler.h>
#include <math.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <resources/yaku_logo.h>
#include <runtime/drawutils.h>
#include <string.h>
#include <types.h>

static window_t windows[64];
static framebuffer_t buffer;
static int cursor_pos_x;
static int cursor_pos_y;
static bool click_down;

void windowmanager_init(void) {
    buffer.width = fb_get_width();
    buffer.height = fb_get_height();
    cursor_pos_x = buffer.width / 2;
    cursor_pos_y = buffer.height / 2;
    // workaround because weird things with the memmap, this is on purpose
    malloc(buffer.width * buffer.height * 4 / PMM_BLOCK_SIZE + 1);
    buffer.buffer = malloc(buffer.width * buffer.height * 4 / PMM_BLOCK_SIZE + 1);
}

void windowmanager_run(void) {
    windowmanager_startup_screen();
    fb_draw_buffer(buffer.buffer);
    scheduler_sleep(1000);

    for (;;) {
        windowmanager_handle_events();
        windowmanager_draw();
        fb_draw_buffer(buffer.buffer);
    }
}

void windowmanager_startup_screen() {
    drawutils_draw_rect_filled(buffer, 0, 0, buffer.width, buffer.height,
                               RGB(255, 255, 255));

    drawutils_draw_image_rgba(buffer, (buffer.width - YAKU_LOGO_WIDTH) / 2,
                              (buffer.height - YAKU_LOGO_HEIGHT) / 2, YAKU_LOGO_WIDTH,
                              YAKU_LOGO_HEIGHT, (const uint32_t*)yaku_logo);
}

void windowmanager_handle_events() {
    input_event_t event;
    if (input_event_get_event(&event)) {
        if (event.kind == EVENT_MOUSE_MOTION) {
            if (click_down) {
                window_t* window =
                    windowmanager_get_window_at(cursor_pos_x, cursor_pos_y);
                if (window != NULL) {
                    window->x += event.mouse_motion.x_rel;
                    window->y -= event.mouse_motion.y_rel;
                    if (window->x < 0) {
                        window->x = 0;
                    }
                    if (window->y < 0) {
                        window->y = 0;
                    }
                    if (window->x >= buffer.width) {
                        window->x = buffer.width - 1;
                    }
                    if (window->y >= buffer.height) {
                        window->y = buffer.height - 1;
                    }
                }
            }

            cursor_pos_x += event.mouse_motion.x_rel;
            cursor_pos_y -= event.mouse_motion.y_rel;
            if (cursor_pos_x < 0) {
                cursor_pos_x = 0;
            }
            if (cursor_pos_y < 0) {
                cursor_pos_y = 0;
            }
            if (cursor_pos_x >= buffer.width) {
                cursor_pos_x = buffer.width - 1;
            }
            if (cursor_pos_y >= buffer.height) {
                cursor_pos_y = buffer.height - 1;
            }
        } else if (event.kind == EVENT_MOUSE_BUTTON) {
            if (event.mouse_button.button == MOUSE_BUTTON_LEFT) {
                if (event.mouse_button.s_kind == MOUSE_BUTTON_DOWN) {
                    click_down = true;
                } else {
                    click_down = false;
                }
            }
        }
    }
}

void windowmanager_draw(void) {
    // background
    drawutils_draw_rect_filled(buffer, 0, 0, buffer.width, buffer.height,
                               RGB(8, 132, 132));

    // windows
    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            continue;
        }
        windowmanager_draw_window(&windows[i]);
    }

    // cursor
    drawutils_draw_rect_filled(buffer, cursor_pos_x - 5, cursor_pos_y - 5, 10, 10,
                               RGB(255, 255, 255));

    // task bar
    drawutils_draw_bordered_rect_default(buffer, 0, buffer.height - 30, buffer.width, 30);
}

void windowmanager_draw_window(window_t* window) {
    // window border

    drawutils_draw_rect(buffer, window->x, window->y, window->width, window->height, 1,
                        RGB(195, 195, 195));

    drawutils_draw_rect(buffer, window->x + 1, window->y + 1, window->width - 2,
                        window->height - 2, 1, RGB(255, 255, 255));

    // copy window content buffer to window
    for (size_t x = 0; x < window->buffer.width; x++) {
        for (size_t y = 0; y < window->buffer.height; y++) {
            // TODO: memcpy instead of repeated function calls and loops
            drawutils_draw_pixel(
                buffer, window->x + 2 + x, window->y + 2 + 30 + y,
                ((uint32_t*)window->buffer.buffer)[x + y * window->buffer.width]);
        }
    }

    // bar
    drawutils_draw_rect_filled(buffer, window->x + 2, window->y + 2, window->width - 4,
                               30, RGB(0, 0, 130));

    // cross box
    drawutils_draw_bordered_rect_default(buffer, window->x + window->width - 27,
                                         window->y + 7, 20, 20);
}

window_t* windowmanager_get_window_at(size_t x, size_t y) {
    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            continue;
        }
        if (x >= windows[i].x && x < windows[i].x + windows[i].width &&
            y >= windows[i].y && y < windows[i].y + windows[i].height) {
            return &windows[i];
        }
    }
    return NULL;
}

window_t* windowmanager_create_window(size_t width, size_t height) {
    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            windows[i].x = 0;
            windows[i].y = 0;
            windows[i].width = width + 4;        // +4 for border
            windows[i].height = height + 4 + 30; // +4 for border, +30 for bar
            windows[i].buffer.height = height;
            windows[i].buffer.width = width;
            windows[i].buffer.buffer =
                malloc(width * height * sizeof(uint32_t) / PMM_BLOCK_SIZE + 1);

            memset(windows[i].buffer.buffer, 0, width * height * sizeof(uint32_t));

            return &windows[i];
        }
    }
    return NULL;
}
