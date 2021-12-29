#pragma once

#include "printf.h"
#include <stdint.h>
#include <types.h>

#define DAYS_OF_YEAR 365
#define SECONDS_PER_DAY 86400

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

bool leap_year(uint16_t year);
uint32_t timestamp(datetime_t* datetime, bool gmt);
datetime_t* datetime_from_timestamp(uint32_t timestamp);
char* strftime(const char* format, datetime_t* datetime);