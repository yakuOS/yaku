#include "cube.h"

#include <math.h>
#include <runtime/windowmanager.h>
#include <string.h>

cube_vec3_t cube_rotate_point(cube_vec3_t point, cube_vec3_t rot) {
    cube_vec3_t result;

    // rotate around x axis
    result.x = point.x;
    result.y = (point.y * cos(rot.x)) - (point.z * sin(rot.x));
    result.z = (point.y * sin(rot.x)) + (point.z * cos(rot.x));

    // rotate around y axis
    result.x = (result.z * sin(rot.y)) + (result.x * cos(rot.y));
    result.z = (result.z * cos(rot.y)) - (result.x * sin(rot.y));

    // rotate around z axis
    result.x = (result.x * cos(rot.z)) - (result.y * sin(rot.z));
    result.y = (result.x * sin(rot.z)) + (result.y * cos(rot.z));

    return result;
}

void cube_main() {
    // TODO: make this work
    return;

    window_t* window = windowmanager_create_window(250, 250, "Cube");

    // clang-format off
    cube_vec3_t cube_corners[8] = {
        {-1, -1, -1},
        { 1, -1, -1},
        {-1, -1,  1},
        { 1, -1,  1},
        {-1,  1, -1},
        { 1,  1, -1},
        {-1,  1,  1},
        { 1,  1,  1}
    };
    // clang-format on

    cube_vec3_t rotated_corners[8];

    cube_vec3_t current_rotation = {0};

    cube_vec3_t rotation_speed = {
        .x = 0.03,
        .y = 0.08,
        .z = 0.13,
    };

    for (;;) {

        current_rotation.x += rotation_speed.x;
        current_rotation.y += rotation_speed.y;
        current_rotation.z += rotation_speed.z;

        memset(window->buffer.buffer, 0,
               window->buffer.width * window->buffer.height * sizeof(uint32_t));

        for (size_t i = 0; i < 8; i++) {
            cube_vec3_t rotated = cube_rotate_point(cube_corners[i], current_rotation);
            size_t screen_x = (size_t)(rotated.x * 50 + 100);
            size_t screen_y = (size_t)(rotated.y * 50 + 100);
            drawutils_draw_rect_filled(window->buffer, screen_x - 3, screen_y, 6, 6,
                                       RGB(255, 0, 0));
        }
    }
}
