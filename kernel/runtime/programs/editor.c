#include "editor.h"

#include <drivers/serial.h>
#include <input/input_event.h>
#include <memory/pmm.h>
#include <resources/keyboard_stringmap.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t index;
    bool shift_pressed;
} editor_state_t;

void editor_on_event(window_t* window, input_event_t event) {
    if (event.kind == EVENT_KEYBOARD) {

        editor_state_t* state = window->data;
        char* keycode = keyboard_stringmap[event.keyboard.keycode][0];
        if (event.keyboard.keycode == KB_SHIFT) {
            serial_printf("%d up or down\n", event.keyboard.s_kind);
            if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN) {
                state->shift_pressed = true;
                return;
            } else {

                state->shift_pressed = false;
                return;
            }
        }
        if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN) {

            if (keycode == NULL) {
                return;
            }

            if (strcmp(keycode, "SPACE") == 0) {
                state->buffer[state->index++] = ' ';
            } else if (strcmp(keycode, "ENTER") == 0) {
                state->buffer[state->index++] = '\n';
            } else if (!strcmp(keycode, "BACKSPACE") && state->index > 0) {
                state->buffer[--state->index] = '\0';
                drawutils_draw_rect_filled(window->buffer, 0, 0, window->buffer.width,
                                           window->buffer.height, RGB(255, 255, 255));
            } else if (strlen(keycode) == 1) {
                keycode = keyboard_stringmap[event.keyboard.keycode]
                                            [(state->shift_pressed ? 1 : 0)];
                state->buffer[state->index++] = keycode[0];
            }
            drawutils_draw_string(window->buffer, 7, 7, state->buffer, 2, RGB(0, 0, 0));
        }
    }
}

void editor_on_close(window_t* window) {
    free(((editor_state_t*)window->data)->buffer);
    free(window->data);
    windowmanager_destroy_window(window);
}

void editor_main(void) {
    window_t* window = windowmanager_create_default_window(400, 400, "Editor");

    editor_state_t* state = malloc(sizeof(editor_state_t));
    state->buffer = malloc(4096);
    state->index = 0;
    memset(state->buffer, 0, 4096);
    window->data = state;
    state->shift_pressed = false;
    window->on_event = &editor_on_event;
    window->on_close = &editor_on_close;

    drawutils_draw_rect_filled(window->buffer, 0, 0, window->buffer.width,
                               window->buffer.height, RGB(255, 255, 255));
}
