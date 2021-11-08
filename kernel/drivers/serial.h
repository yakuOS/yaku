#pragma once

#include <types.h>

#define SERIAL_PORT 0x3f8

void serial_init(void);
bool serial_is_transmit_empty(void);
void serial_putc(char c);
void serial_puts(char* str);
int serial_printf(char* format, ...);
