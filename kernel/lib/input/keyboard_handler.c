#include "keyboard_handler.h"

void keyboard_handler(char keystroke) {
    input_event_t event;
    event.kind = EVENT_KEYBOARD;
    if (keystroke >= 0) {
        event.keyboard.s_kind = KEYBOARD_KEY_DOWN;
    } else {
        event.keyboard.s_kind = KEYBOARD_KEY_UP;
    }
    if (keystroke >= 0) {
        event.keyboard.keycode = keystroke;
    } else {
        event.keyboard.keycode = -keystroke;
    }
    input_event_append_event(event);
}
