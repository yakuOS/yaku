#pragma once

#include <stdint.h>
#include <types.h>
#include <lib/datetime.h>

#define RTC_REGISTER_PORT 0x70
#define RTC_DATA_PORT     0x71

#define RTC_SECONDS      0x00
#define RTC_MINUTES      0x02
#define RTC_HOURS        0x04
#define RTC_WEEKDAY      0x06
#define RTC_DAY_OF_MONTH 0x07
#define RTC_MONTH        0x08
#define RTC_YEAR         0x09
#define RTC_CENTURY      0x32

#define RTC_STATUS_REGISTER_A 0x0A
#define RTC_STATUS_REGISTER_B 0x0B
#define RTC_STATUS_REGISTER_C 0x0C

uint8_t rtc_bcd_to_bin(uint8_t bcd_data);
uint8_t rtc_read_register(uint8_t reg);
void rtc_read_time(datetime_t* rtc_time);
void rtc_write_register(uint8_t reg, uint8_t data);
void rtc_debug_print();