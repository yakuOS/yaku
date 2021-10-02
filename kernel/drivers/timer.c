#include "pit.h"

#include <types.h>

void timer_sleep_ticks(uint16_t ticks) {
    uint32_t expected_tick = pit_tick_get() + ticks;

    while (pit_tick_get() < expected_tick) {
        asm volatile("nop");
    }
}
