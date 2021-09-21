#pragma once
#include "types.h"

// Sends a 8/16/32-bit value on a I/O location
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Receives a 8/16/32-bit value from an I/O location
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Wait a very small amount of time (1 to 4 microseconds, generally). Useful for
// implementing a small delay for PIC remapping on old hardware or generally as a simple
// but imprecise wait.
static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile("pushf\n\t"
                 "pop %0"
                 : "=g"(flags));
    return flags & (1 << 9);
}
