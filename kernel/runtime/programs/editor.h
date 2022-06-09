#pragma once

#include <types.h>

typedef struct {
    char* buffer;
    size_t index;
} editor_state_t;

void editor_main(void);
