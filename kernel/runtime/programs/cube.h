#pragma once

typedef struct {
    double x, y, z;
} cube_vec3_t;

void cube_main();
cube_vec3_t cube_rotate_point(cube_vec3_t point, cube_vec3_t rot);
