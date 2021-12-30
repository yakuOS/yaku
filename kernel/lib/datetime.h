#pragma once

#include <types.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;
    uint8_t day_of_month;
    uint8_t month;
    uint16_t year;
    uint8_t century;
} datetime_t;

uint32_t datetime_to_timestamp(datetime_t* datetime, bool gmt);
datetime_t* datetime_from_timestamp(uint32_t timestamp);
char* datetime_strftime(const char* format, datetime_t* datetime);
