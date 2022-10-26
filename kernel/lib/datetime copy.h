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
uint32_t datetime_get_timestamp();
uint32_t datetime_to_timestamp(datetime_t* datetime, bool gmt);
void datetime_from_timestamp(uint32_t timestamp, datetime_t* final_date);
void datetime_strftime(datetime_t* datetime, char* format, char* dest, size_t dest_size);
