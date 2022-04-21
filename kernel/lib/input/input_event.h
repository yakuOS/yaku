#pragma once
#include <resources/keyboard_keycode.h>
#include <types.h>

typedef enum {
    EVENT_KEYBOARD,
    EVENT_MOUSE_MOTION,
    EVENT_MOUSE_BUTTON,
    EVENT_MOUSE_WHEEL
} input_event_kind_t;

typedef enum {
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_UP,
} input_event_keyboard_kind_t;

typedef enum {
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
} input_event_mouse_button_kind_t;

typedef enum {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
} input_event_mouse_button_t;

typedef enum {
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN,
} input_event_mouse_wheel_kind_t;

typedef struct {
    input_event_keyboard_kind_t s_kind;
    keyboard_keycode_t keycode;
} input_event_keyboard_event_t;

typedef struct {
    int32_t x_rel;
    int32_t y_rel;
} input_event_mouse_motion_event_t;

typedef struct {
    input_event_mouse_button_kind_t s_kind;
    input_event_mouse_button_t button;
} input_event_mouse_button_event_t;

typedef struct {
    input_event_mouse_wheel_kind_t s_kind;
} input_event_mouse_wheel_event_t;

typedef struct {
    input_event_kind_t kind;
    union {
        input_event_keyboard_event_t keyboard;
        input_event_mouse_motion_event_t mouse_motion;
        input_event_mouse_button_event_t mouse_button;
        input_event_mouse_wheel_event_t mouse_wheel;
    };
} input_event_t;

void input_event_append_event(input_event_t event);
int input_event_get_event(input_event_t* event);
