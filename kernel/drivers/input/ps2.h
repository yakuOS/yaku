#pragma once

#include <types.h>

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

#define PS2_DISABLE_PORT2 0xA7
#define PS2_ENABLE_PORT2 0xA8
#define PS2_DISABLE_PORT1 0xAD
#define PS2_ENABLE_PORT1 0xAE

#define PS2_READ_CONFIG 0x20
#define PS2_WRITE_CONFIG 0x60

#define PS2_PORT1_IRQ 0x01
#define PS2_PORT2_IRQ 0x02
#define PS2_PORT1_TLATE 0x40

uint8_t ps2_wait_input(void);
uint8_t ps2_wait_output(void);
void ps2_disable(void);
void ps2_enable(void);
void ps2_write_command(uint8_t cmdbyte);
uint8_t ps2_write_command_read_data(uint8_t cmdbyte);
void ps2_write_command_arg(uint8_t cmdbyte, uint8_t arg);
uint8_t ps2_read_status(void);
uint8_t ps2_read_data(void);
uint8_t ps2_write_data(uint8_t cmdbyte);
uint8_t ps2_write_data_arg(uint8_t cmdbyte, uint8_t arg);
void ps2_init(void);


extern bool ps2_data_response_req;
extern uint8_t ps2_response_count;