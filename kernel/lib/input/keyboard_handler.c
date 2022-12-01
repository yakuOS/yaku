#include "keyboard_handler.h"
#include <drivers/serial.h>
void keyboard_handler(char keystroke) {
    serial_printf("keychar %d\n",keystroke);
    input_event_t event;
    event.kind = EVENT_KEYBOARD;
    if (keystroke > 0) {
        event.keyboard.s_kind = KEYBOARD_KEY_DOWN;
        event.keyboard.keycode = keystroke;
    } else {
        serial_printf("up\n");
        event.keyboard.s_kind = KEYBOARD_KEY_UP;
        event.keyboard.keycode = -keystroke;
    }
    input_event_append_event(event);
}
