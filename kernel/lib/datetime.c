#include "datetime.h"
#include "drivers/serial.h"
#include "drivers/vga_text.h"
#include "stdint.h"
#include "types.h"
#include <string.h>

#define LEAP_YEAR(YEAR) ((YEAR > 0) && !(YEAR % 4) && ((YEAR % 100) || !(YEAR % 400)))

static const uint8_t months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t timestamp(datetime_t* datetime, bool gmt) {

    uint32_t timestamp = 0;

    timestamp += (datetime->year - 1970) * (SECONDS_PER_DAY * DAYS_OF_YEAR);

    for (uint32_t year = 1970; year < datetime->year; year++)
        if (LEAP_YEAR(year))
            timestamp += SECONDS_PER_DAY;

    for (uint8_t month = 1; month < datetime->month; month++)
        if (LEAP_YEAR(datetime->year) && month == 2)
            timestamp += SECONDS_PER_DAY * 29;
        else
            timestamp += SECONDS_PER_DAY * months[month - 1];

    timestamp += (datetime->day_of_month - 1) * SECONDS_PER_DAY;
    timestamp += datetime->hour * 3600;
    timestamp += datetime->minute * 60;
    timestamp += datetime->second;

    return gmt ? timestamp : timestamp - 7200;
}

void datetime_from_timestamp(uint32_t timestamp, datetime_t* final_date) {

    long int currYear, daysTillNow, extraTime, extraDays, index, date, month, hours,
        minutes, seconds, flag = 0;

    daysTillNow = timestamp / SECONDS_PER_DAY;
    extraTime = timestamp % SECONDS_PER_DAY;
    currYear = 1970;

    while (daysTillNow >= 365) {
        if (LEAP_YEAR(currYear))
            daysTillNow -= 366;
        else
            daysTillNow -= 365;
        currYear += 1;
    }

    extraDays = daysTillNow + 1;

    if (LEAP_YEAR(currYear))
        flag = 1;

    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            } else {
                if (extraDays - months[index] < 0)
                    break;
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
        if (month == 2 && flag == 1)
            date = 29;
        else
            date = months[month - 1];
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

static void convert(uint16_t value, char* out, int* c) {

    char buf[5];
    snprintf(buf, 5, "%02i", value);

    uint32_t len = strlen(buf);

    for (uint32_t i = 0; i < len; i++) {
        out[(*c)++] = buf[i];
    }
}

void strftime(const char* format, datetime_t* datetime, char* dest, size_t dest_size) {

    if (dest_size < strlen(format) + 2) {
        return;
    }

    uint8_t day = datetime->day_of_month;
    uint8_t month = datetime->month;
    uint16_t year = datetime->year;
    uint8_t hours = datetime->hour;
    uint8_t minutes = datetime->minute;
    uint8_t second = datetime->second;

    int c = 0;
    int format_length = strlen(format);
    for (int i = 0; i < format_length; i++) {

        if (format[i] != '%') {
            dest[c++] = format[i];
        }

        if (format[i] == '%') {
            switch (format[i + 1]) {
            case 'd':
                convert(day, dest, &c);
                i++;
                break;
            case 'm':
                convert(month, dest, &c);
                i++;
                break;
            case 'y':
                convert(year, dest, &c);
                i++;
                break;
            case 'M':
                convert(minutes, dest, &c);
                i++;
                break;
            case 'H':
                convert(hours, dest, &c);
                i++;
                break;
            case 'S':
                convert(second, dest, &c);
                i++;
                break;
            }
        }
    }
    dest[c] = '\0';
}
