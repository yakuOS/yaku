#pragma once
#include <resources/keyboard_enum.h>
#include <types.h>

enum input_event_kind {
    EVENT_KEYBOARD,
    EVENT_MOUSE_MOTION,
    EVENT_MOUSE_BUTTON,
    EVENT_MOUSE_WHEEL
};
enum input_event_keyboard_kind {
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_UP,
};
enum input_event_mouse_button_kind {
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
};
enum input_event_mouse_button {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
};
enum input_event_mouse_wheel_kind {
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN,
};
struct input_event_keyboard_event {
    enum input_event_keyboard_kind s_kind;
    enum keycode_enum keycode;
};
struct input_event_mouse_motion_event {
    int32_t x_rel;
    int32_t y_rel;
};
struct input_event_mouse_button_event {
    enum input_event_mouse_button_kind s_kind;
    enum input_event_mouse_button button;
};
struct input_event_mouse_wheel_event {
    enum input_event_mouse_wheel_kind s_kind;
};
typedef struct {
    enum input_event_kind kind;
    union {
        struct input_event_keyboard_event keyboard;
        struct input_event_mouse_motion_event mouse_motion;
        struct input_event_mouse_button_event mouse_button;
        struct input_event_mouse_wheel_event mouse_wheel;
    };
} input_event_t;

void input_event_append_event(input_event_t event);
input_event_t* input_event_get_event();
