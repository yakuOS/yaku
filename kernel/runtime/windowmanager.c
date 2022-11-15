#include "windowmanager.h"

#include <drivers/fb.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <lib/datetime.h>
#include <lib/input/mouse_handler.h>
#include <math.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <resources/cursor.h>
#include <resources/yaku_logo.h>
#include <runtime/drawutils.h>
#include <string.h>
#include <types.h>

// programs (will be read from hard drive later)
#include <runtime/programs/editor.h>
#include <runtime/programs/gradient.h>
#include <runtime/programs/tbz.h>
#include <runtime/programs/tictactoe.h>

static window_t windows[64];
static window_t* current_window;
static framebuffer_t buffer;
static int cursor_pos_x;
static int cursor_pos_y;
static bool click_down_bar;

void windowmanager_main(void) {
    // init
    buffer.width = fb_get_width();
    buffer.height = fb_get_height();
    cursor_pos_x = buffer.width / 2;
    cursor_pos_y = buffer.height / 2;
    // workaround because weird things with the memmap, this is on purpose
    malloc(buffer.width * buffer.height * 4);
    malloc(buffer.width * buffer.height * 4);
    buffer.buffer = malloc(buffer.width * buffer.height * 4);

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
            // move window
            if (current_window && click_down_bar) {
                if (current_window != NULL) {
                    current_window->x += event.mouse_motion.x_rel;
                    current_window->y -= event.mouse_motion.y_rel;
                    if (current_window->x < 0) {
                        current_window->x = 0;
                    }
                    if (current_window->y < 0) {
                        current_window->y = 0;
                    }
                    if (current_window->x >= buffer.width) {
                        current_window->x = buffer.width - 1;
                    }
                    if (current_window->y >= buffer.height) {
                        current_window->y = buffer.height - 1;
                    }
                }
            }

            // move cursor
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
                    current_window =
                        windowmanager_get_window_at(cursor_pos_x, cursor_pos_y);
                    if (current_window != NULL) {
                        // check if click on cross
                        if (cursor_pos_x >=
                                current_window->x + current_window->width - 27 &&
                            cursor_pos_x <=
                                current_window->x + current_window->width - 7 &&
                            cursor_pos_y >= current_window->y + 7 &&
                            cursor_pos_y <= current_window->y + 27) {
                            // run window close handler (free resources etc)
                            if (current_window->on_close != NULL) {
                                current_window->on_close(current_window);
                            }
                            //  no handler, just destroy window
                            else {
                                windowmanager_destroy_window(current_window);
                            }
                        }
                        // check if click on bar (move window)
                        else if (cursor_pos_y - current_window->y < 30 + 2) {
                            click_down_bar = true;
                        }
                        // forward mouse click event if neither cross nor bar
                        else if (current_window->on_event != NULL) {
                            current_window->on_event(current_window, event);
                        }
                    }
                } else {
                    click_down_bar = false;
                }
            } else {
                // forward mouse button evenet
                if (current_window != NULL && current_window->on_event != NULL) {
                    current_window->on_event(current_window, event);
                }
            }
        } else if (event.kind == EVENT_KEYBOARD) {

            if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN &&
                event.keyboard.keycode == KB_2) {
                task_add(&gradient_main, TASK_PRIORITY_LOW, 0);
            } else if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN &&
                       event.keyboard.keycode == KB_3) {
                task_add(&tictactoe_main, TASK_PRIORITY_LOW, 0);
            } else if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN &&
                       event.keyboard.keycode == KB_4) {
                task_add(&editor_main, TASK_PRIORITY_LOW, 0);
            } else if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN &&
                       event.keyboard.keycode == KB_5) {
                task_add(&tbz_main, TASK_PRIORITY_LOW, 0);
            } else if (current_window != NULL && current_window->on_event != NULL) {
                current_window->on_event(current_window, event);
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
        if (windows[i].width == 0 || &windows[i] == current_window) {
            continue;
        }
        windowmanager_draw_window(&windows[i]);
    }

    if (current_window) {
        windowmanager_draw_window(current_window);
    }

    // task bar
    drawutils_draw_bordered_rect_default(buffer, 0, buffer.height - 30, buffer.width, 30);

    // time and date
    datetime_t datetime;
    rtc_read_time(&datetime);

    char time_str[9];
    char date_str[9];
    datetime_strftime(&datetime, "%H:%M:%S", time_str, 9);
    datetime_strftime(&datetime, "%d.%m.%Y", date_str, 9);

    drawutils_draw_string(buffer, buffer.width - (8 * 8 + 5), buffer.height - 25,
                          time_str, 1, RGB(255, 255, 255));
    drawutils_draw_string(buffer, buffer.width - (8 * 8 + 5), buffer.height - 13,
                          date_str, 1, RGB(255, 255, 255));

    // cursor
    drawutils_draw_image_rgba(buffer, cursor_pos_x, cursor_pos_y, CURSOR_WIDTH,
                              CURSOR_HEIGHT, (const uint32_t*)cursor);
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

    // title
    drawutils_draw_string(buffer, window->x + 2 + 7, window->y + 2 + 7, window->title, 2,
                          RGB(255, 255, 255));

    // cross box
    drawutils_draw_bordered_rect_default(buffer, window->x + window->width - 27,
                                         window->y + 7, 20, 20);
}

window_t* windowmanager_get_window_at(size_t x, size_t y) {
    // current window is rendered on top, so it has priority
    if (current_window && x >= current_window->x &&
        x < current_window->x + current_window->width && y >= current_window->y &&
        y < current_window->y + current_window->height) {
        return current_window;
    }

    // other windows, in reverse order
    for (int i = 63; i >= 0; i--) {
        if (windows[i].width == 0) {
            continue;
        }
        if (x >= windows[i].x && x < windows[i].x + windows[i].width &&
            y >= windows[i].y && y < windows[i].y + windows[i].height) {
            return &windows[i];
        }
    }

    // no window found
    return NULL;
}

window_t* windowmanager_create_window(size_t width, size_t height, size_t x, size_t y,
                                      char* title) {
    for (size_t i = 0; i < 64; i++) {
        if (windows[i].width == 0) {
            windows[i].x = x;
            windows[i].y = y;
            windows[i].width = width + 4;        // +4 for border
            windows[i].height = height + 4 + 30; // +4 for border, +30 for bar
            windows[i].title = title;
            windows[i].on_event = NULL;
            windows[i].on_close = NULL;
            windows[i].buffer.height = height;
            windows[i].buffer.width = width;
            windows[i].buffer.buffer = malloc(width * height * sizeof(uint32_t));

            memset(windows[i].buffer.buffer, 0, width * height * sizeof(uint32_t));

            return &windows[i];
        }
    }
    return NULL;
}

window_t* windowmanager_create_default_window(size_t width, size_t height, char* title) {
    return windowmanager_create_window(width, height, 0, 0, title);
}

void windowmanager_destroy_window(window_t* window) {
    if (window == current_window) {
        current_window = NULL;
    }

    if (window->buffer.buffer) {
        free(window->buffer.buffer);
    }

    memset(window, 0, sizeof(window_t));
}

void windowmanager_get_relative_cursor_pos(window_t* window, size_t* x, size_t* y) {
    // TODO: potentially bogus values if cursor is outside of window
    *x = cursor_pos_x - window->x - 2;
    *y = cursor_pos_y - window->y - 2 - 30;
}
