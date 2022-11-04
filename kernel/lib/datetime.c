#include "datetime.h"

#include <drivers/serial.h>
#include <printf.h>
#include <string.h>
#include <types.h>
#include <drivers/rtc.h>
#define IS_LEAP_YEAR(YEAR) ((YEAR > 0) && !(YEAR % 4) && ((YEAR % 100) || !(YEAR % 400)))
#define DAYS_PER_YEAR 365
#define SECONDS_PER_DAY 86400

static const uint8_t months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t datetime_to_timestamp(datetime_t* datetime, bool gmt) {

    uint32_t timestamp = 0;

    timestamp += (datetime->year - 1970) * (SECONDS_PER_DAY * DAYS_PER_YEAR);

    for (uint32_t year = 1970; year < datetime->year; year++) {
        if (IS_LEAP_YEAR(year)) {
            timestamp += SECONDS_PER_DAY;
        }
    }

    for (uint8_t month = 1; month < datetime->month; month++) {
        if (IS_LEAP_YEAR(datetime->year) && month == 2) {
            timestamp += SECONDS_PER_DAY * 29;

        } else {
            timestamp += SECONDS_PER_DAY * months[month - 1];
        }
    }

    timestamp += (datetime->day_of_month - 1) * SECONDS_PER_DAY;
    timestamp += datetime->hour * 3600;
    timestamp += datetime->minute * 60;
    timestamp += datetime->second;

    return gmt ? timestamp : timestamp - 7200;
}
uint32_t datetime_get_timestamp() {
    datetime_t rtc_time;
    rtc_read_time(&rtc_time);
    uint32_t timestamp = datetime_to_timestamp(&rtc_time, true);
    return timestamp;
}
void datetime_from_timestamp(uint32_t timestamp, datetime_t* final_date) {

    long int currYear, daysTillNow, extraTime, extraDays, index, date, month, hours,
        minutes, seconds, flag = 0;

    daysTillNow = timestamp / SECONDS_PER_DAY;
    extraTime = timestamp % SECONDS_PER_DAY;
    currYear = 1970;

    while (daysTillNow >= 365) {
        if (IS_LEAP_YEAR(currYear)) {
            daysTillNow -= 366;

        } else {
            daysTillNow -= 365;
        }
        currYear += 1;
    }

    extraDays = daysTillNow + 1;

    if (IS_LEAP_YEAR(currYear)) {
        flag = 1;
    }

    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0) {
                    break;
                }
                month += 1;
                extraDays -= 29;
            } else {
                if (extraDays - months[index] < 0) {
                    break;
                }
                month += 1;
                extraDays -= months[index];
            }
            index += 1;
        }
    } else {
        while (true) {
            if (extraDays - months[index] < 0) {
                break;
            }
            month += 1;
            extraDays -= months[index];
            index += 1;
        }
    }

    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    } else {
        if (month == 2 && flag == 1) {
            date = 29;
        } else {
            date = months[month - 1];
        }
    }

    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    seconds = (extraTime % 3600) % 60;

    final_date->year = currYear;
    final_date->month = month;
    final_date->day_of_month = date;
    final_date->hour = hours;
    final_date->minute = minutes;
    final_date->second = seconds;
}

void datetime_strftime(datetime_t* datetime, char* format, char* dest, size_t dest_size) {
    size_t format_length = strlen(format);
    size_t dest_idx = 0;
    for (size_t i = 0; i < format_length; i++) {
        if (dest_idx >= dest_size - 1) {
            break;
        }

        if (format[i] == '%') {
            uint16_t value = 0;
            switch (format[++i]) {
            case 'd':
                value = datetime->day_of_month;
                break;
            case 'm':
                value = datetime->month;
                break;
            case 'y':
                value = datetime->year;
                break;
            case 'Y':
                value = datetime->year - 2000;
                break;
            case 'M':
                value = datetime->minute;
                break;
            case 'H':
                value = datetime->hour;
                break;
            case 'S':
                value = datetime->second;
                break;
            }

            dest_idx += snprintf(dest + dest_idx, dest_size - dest_idx, "%02i", value);
        } else {
            dest[dest_idx++] = format[i];
        }
    }

    dest[dest_idx] = '\0';
}
