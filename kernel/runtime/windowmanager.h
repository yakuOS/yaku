#pragma once

#include <input/input_event.h>
#include <runtime/drawutils.h>
#include <types.h>

typedef struct window {
    int x, y;
    size_t width, height, index;
    framebuffer_t buffer;
    char* title;
    void (*on_event)(struct window* window, input_event_t event);
    void* data;
} window_t;

void windowmanager_init(void);
void windowmanager_event_hander(input_event_t* event);
void windowmanager_run(void);
void windowmanager_startup_screen(void);
void windowmanager_draw(void);
void windowmanager_draw_window(window_t* window);
window_t* windowmanager_get_window_at(size_t x, size_t y);
window_t* windowmanager_create_window(size_t width, size_t height, char* title);
void draw();
