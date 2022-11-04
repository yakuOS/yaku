#include "input_event.h"
#include <runtime/windowmanager.h>
#include <string.h>

input_event_t buffer[256];
uint8_t buffer_pointer = 0;
uint8_t last_read_buffer_pointer = 0;

// overwrites buffer at buffer pointer with event, increases buffer pointer
void input_event_append_event(input_event_t event) {
    // buffer[buffer_pointer] = event;
    // buffer_pointer++;
    windowmanager_event_hander(&event);
}

// returns 0 if no event is available, otherwise  1
int input_event_get_event(input_event_t* event) {
    if (buffer_pointer == last_read_buffer_pointer) {
        return 0;
    }
    memcpy(event, buffer + last_read_buffer_pointer++, sizeof(input_event_t));
    return 1;
}
