#pragma once
#include <types.h>

/**
 * @buffer: array in which key"events" will be stored
 */
typedef struct read_input_listener {
    uint8_t buffer[256];
    uint8_t current_buffer_pointer;
    uint8_t last_read_buffer_pointer;
    void (*keystroke_handler)(char keystroke, struct read_input_listener* this);
    uint8_t (*get_keystrokes)(struct read_input_listener* this);
} read_input_listener;
