#include "input_event.h"

input_event_t buffer[256];
uint8_t buffer_pointer = 0;
uint8_t last_read_buffer_pointer = 0;

// overwrites buffer at buffer pointer with event, increases buffer pointer
void input_event_append_event(input_event_t event) {
    buffer[buffer_pointer] = event;
    buffer_pointer++;
}

/**
 * @return input_event_t* pointer to event at last-read-buffer-pointer
 * no new event = Null
 */
input_event_t* input_event_get_event() {
    if (buffer_pointer == last_read_buffer_pointer) {
        return NULL;
    }
    last_read_buffer_pointer++;
    return &buffer[last_read_buffer_pointer - 1];
}
