#include "editor.h"

#include <drivers/serial.h>
#include <memory/pmm.h>
#include <resources/keyboard_stringmap.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>
#include <string.h>

void editor_on_event(window_t* window, input_event_t event) {
    if (event.kind == EVENT_KEYBOARD) {
        if (event.keyboard.s_kind == KEYBOARD_KEY_DOWN) {
            editor_state_t* state = window->data;

            char* keycode = keyboard_stringmap[event.keyboard.keycode][0];
            if (keycode == NULL) {
                return;
            }

            if (strcmp(keycode, "SPACE") == 0) {
                state->buffer[state->index++] = ' ';
            } else if (strcmp(keycode, "ENTER") == 0) {
                state->buffer[state->index++] = '\n';
            } else if (strlen(keycode) == 1) {
                state->buffer[state->index++] = keycode[0];
            }
        }
    }
}

void editor_main(void) {
    window_t* window = windowmanager_create_window(400, 400, "Editor");

    void* mem = malloc(5 * 4096);
    editor_state_t* state = mem;
    state->buffer = mem + sizeof(editor_state_t);
    memset(state->buffer, 0, mem - sizeof(editor_state_t));
    window->data = state;

    window->on_event = &editor_on_event;

    drawutils_draw_rect_filled(window->buffer, 0, 0, window->buffer.width,
                               window->buffer.height, RGB(255, 255, 255));

    for (;;) {
        drawutils_draw_string(window->buffer, 7, 7, state->buffer, 2, RGB(0, 0, 0));
    }
}
