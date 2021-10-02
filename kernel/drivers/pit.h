#pragma once
#include <types.h>

void pit_increase(void);
void pit_init(uint32_t frequency);
uint32_t pit_tick_get(void);
uint32_t pit_read(void);
