#include <types.h>
#include "pit.h"



void timer_sleep(uint16_t ticks_sleep){
    uint32_t expected_tick;

    expected_tick = pit_tick_get() + ticks_sleep;
    while (pit_tick_get() < expected_tick);
}
