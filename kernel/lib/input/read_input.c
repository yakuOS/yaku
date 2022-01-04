#include "read_input.h"

#include <drivers/input/input_device.h>
#include <drivers/serial.h>
#include <types.h>

#include <resources/keyboard_stringmap.h>

read_input_listener* listeners[256];
bool listener_existent = false;
void read_input_handle_keystroke(char keystroke, read_input_listener* this) {
    this->buffer[(this->current_buffer_pointer + 1) % 256] = keystroke;
    this->current_buffer_pointer += 1;
}

/**
 * @buffer: The buffer to store the keystrokes in. (length 256)
 */
void read_input_get_keystrokes(read_input_listener* this, uint8_t* buffer[256]) {
    uint8_t length =
        (this->current_buffer_pointer - this->last_read_buffer_pointer) < 0
            ? -(this->current_buffer_pointer - this->last_read_buffer_pointer)
            : (this->current_buffer_pointer - this->last_read_buffer_pointer); // math.abs
    for (int i = 0; i < length; i++) {
        buffer[i] = this->buffer[(this->last_read_buffer_pointer + i) % 256];
    }
    this->last_read_buffer_pointer = this->current_buffer_pointer;
}

void read_input_handle(char keystroke) {
    for (int i = 0; i < 256; i++) {
        if (listeners[i] != NULL) {
            listeners[i]->keystroke_handler(keystroke, listeners[i]);
        }
    }
}

void read_input_init_listener(read_input_listener* listener) {
    listener->current_buffer_pointer = 0;
    listener->last_read_buffer_pointer = 0;
    listener->keystroke_handler = &read_input_handle_keystroke;
    listener->get_keystrokes = &read_input_get_keystrokes;
    if (!listener_existent) {
        input_device_add_listener(0, "read_input_library", &read_input_handle);
        listener_existent = true;
    }
    for (int i = 0; i < 255; i++) {
        if (listeners[i] == NULL) {
            listeners[i] = listener;
            return;
        }
    }
}
