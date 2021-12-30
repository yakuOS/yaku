#include "datetime.h"

#include <printf.h>
#include <types.h>

#define DAYS_PER_YEAR 365
#define SECONDS_PER_DAY 86400

#define IS_LEAP_YEAR(YEAR) ((YEAR > 0) && !(YEAR % 4) && ((YEAR % 100) || !(YEAR % 400)))

static const uint8_t months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t datetime_to_timestamp(datetime_t* datetime, bool gmt) {

    uint32_t timestamp = 0;

    timestamp += (datetime->year - 1970) * (SECONDS_PER_DAY * DAYS_PER_YEAR);

    for (uint32_t year = 1970; year < datetime->year; year++) {
        if (IS_LEAP_YEAR(year)) {
            timestamp += SECONDS_PER_DAY;
        }
    }

    for (uint8_t month = 1; month < datetime->month; month++)
        if (IS_LEAP_YEAR(datetime->year) && month == 2) {
            timestamp += SECONDS_PER_DAY * 29;

        } else {
            timestamp += SECONDS_PER_DAY * months[month - 1];
        }

    timestamp += (datetime->day_of_month - 1) * SECONDS_PER_DAY;
    timestamp += datetime->hour * 3600;
    timestamp += datetime->minute * 60;
    timestamp += datetime->second;

    return gmt ? timestamp : timestamp - 7200;
}

datetime_t* datetime_from_timestamp(uint32_t timestamp) {
    static datetime_t* final_date;

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

    return final_date;
}

static char* string;
static int string_count = 0;

static void convert(uint16_t value_to_convert) {
    uint8_t buf_count = 0;
    static char date_buffer[10];
    sprintf(date_buffer, "%i", value_to_convert);

    while (date_buffer[buf_count]) {
        string[string_count++] = date_buffer[buf_count++];
    }
}

char* datetime_strftime(const char* format, datetime_t* datetime) {

    uint8_t day = datetime->day_of_month;
    uint8_t month = datetime->month;
    uint16_t year = datetime->year;
    uint8_t hours = datetime->hour;
    uint8_t minutes = datetime->minute;
    uint8_t second = datetime->second;

    while (*format) {

        if (*format != '%') {
            string[string_count++] = *format;
        }

        if (*format == '%') {
            format++;

            switch (*format) {
            case 'd':
                convert(day);
                break;
            case 'm':
                convert(month);
                break;
            case 'y':
                convert(year);
                break;
            case 'H':
                convert(hours);
                break;
            case 'M':
                convert(minutes);
                break;
            case 'S':
                convert(second);
                break;
            }
        }

        format++;
    }

    string[string_count++] = '\0';

    return string;
}
