#include "windowmanager.h"

#include <drivers/fb.h>
#include <drivers/serial.h>
#include <lib/input/keyboard_handler.h>
#include <lib/input/mouse_handler.h>
#include <math.h>
#include <memory/pmm.h>
#include <string.h>
#include <types.h>

#define RGB(r, g, b) ((0 << 24) | ((r) << 16) | ((g) << 8) | (b))

static window_t windows[64];
static size_t buffer_width;
static size_t buffer_height;
static uint32_t* buffer;
static int cursor_pos_x;
static int cursor_pos_y;
static bool click_down;

void windowmanager_init(void) {
    buffer_width = fb_get_width();
    buffer_height = fb_get_height();
    cursor_pos_x = buffer_width / 2;
    cursor_pos_y = buffer_height / 2;
    // workaround because weird things with the memmap, this is on purpose
    buffer = malloc(buffer_width * buffer_height * 4 / PMM_BLOCK_SIZE + 1);
    buffer = malloc(buffer_width * buffer_height * 4 / PMM_BLOCK_SIZE + 1);
}

void windowmanager_run(void) {
    input_event_t event;
    for (;;) {
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
                        if (window->x >= buffer_width) {
                            window->x = buffer_width - 1;
                        }
                        if (window->y >= buffer_height) {
                            window->y = buffer_height - 1;
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
                if (cursor_pos_x >= buffer_width) {
                    cursor_pos_x = buffer_width - 1;
                }
                if (cursor_pos_y >= buffer_height) {
                    cursor_pos_y = buffer_height - 1;
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

        windowmanager_draw();
        fb_draw_buffer(buffer);
    }
}

void windowmanager_draw(void) {

    windowmanager_draw_rect_filled(0, 0, buffer_width, buffer_height, RGB(8, 132, 132));

    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            continue;
        }
        windowmanager_draw_window(&windows[i]);
    }

    windowmanager_draw_rect_filled(cursor_pos_x - 5, cursor_pos_y - 5, 10, 10,
                                   0xFF0000FF);

    windowmanager_draw_bordered_rect_default(0, buffer_height - 30, buffer_width, 30);
}

void windowmanager_draw_window(window_t* window) {
    // window border and bg
    windowmanager_draw_bordered_rect_default(window->x, window->y, window->width,
                                             window->height);

    // bar
    windowmanager_draw_rect_filled(window->x + 2, window->y + 2, window->width - 4, 30,
                                   RGB(0, 0, 130));

    // cross box
    windowmanager_draw_bordered_rect_default(window->x + window->width - 27,
                                             window->y + 7, 20, 20);
}

void windowmanager_draw_bordered_rect(int x, int y, int width, int height,
                                      uint32_t color_outer_border,
                                      uint32_t color_inner_border,
                                      uint32_t color_content) {
    windowmanager_draw_rect(x, y, width, height, 1, color_outer_border);
    windowmanager_draw_rect(x + 1, y + 1, width - 2, height - 2, 1, color_inner_border);
    windowmanager_draw_rect_filled(x + 2, y + 2, width - 4, height - 4, color_content);
}

void windowmanager_draw_bordered_rect_default(int x, int y, int width, int height) {
    windowmanager_draw_bordered_rect(x, y, width, height, RGB(195, 195, 195),
                                     RGB(255, 255, 255), RGB(195, 195, 195));
}

void windowmanager_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        windowmanager_draw_pixel(x1, y1, color);
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

void windowmanager_draw_rect_filled(size_t x, size_t y, size_t width, size_t height,
                                    uint32_t color) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            windowmanager_draw_pixel(x + j, y + i, color);
        }
    }
}

void windowmanager_draw_rect(size_t x, size_t y, size_t width, size_t height,
                             size_t thickness, uint32_t color) {

    for (size_t i = 0; thickness > 0; thickness--, i++) {
        for (size_t j = 0; j < width; j++) {
            windowmanager_draw_pixel(x + j, y + i, color);
            windowmanager_draw_pixel(x + j, y + height - i - 1, color);
        }

        for (size_t j = 0; j < height; j++) {
            windowmanager_draw_pixel(x + i, y + j, color);
            windowmanager_draw_pixel(x + width - i - 1, y + j, color);
        }
    }
}

void windowmanager_draw_pixel(size_t x, size_t y, uint32_t color) {
    buffer[y * buffer_width + x] = color;
}

window_t* windowmanager_create_window(size_t width, size_t height) {
    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            windows[i].x = 0;
            windows[i].y = 0;
            windows[i].width = width;
            windows[i].height = height;
            return &windows[i];
        }
    }
    return NULL;
}
