#include "rtc.h"
#include "drivers/serial.h"
#include "vga_text.h"
#include <drivers/timer.h>
#include <io.h>
#include <lib/datetime.h>
#include <printf.h>
#include <stdint.h>
#include <types.h>

uint8_t rtc_update_in_progress_flag() {
    return rtc_read_register(RTC_STATUS_REGISTER_A) >> 6;
}

uint8_t rtc_bcd_to_bin(uint8_t bcd_data) {
    return ((bcd_data >> 4) * 10) + (bcd_data & 0x0F);
}

uint8_t rtc_read_register(uint8_t reg) {
    io_outb(RTC_REGISTER_PORT, reg);
    return io_inb(0x71);
}

void rtc_write_register(uint8_t reg, uint8_t data) {
    io_outb(RTC_REGISTER_PORT, reg);
    io_outb(RTC_DATA_PORT, data);
}

void rtc_read_time(datetime_t* rtc_time) {

    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_weekday;
    uint8_t last_day_of_month;
    uint8_t last_month;
    uint16_t last_year;
    uint8_t last_century;

    while (rtc_update_in_progress_flag() != 0)
        ;

    rtc_time->second = rtc_read_register(RTC_SECONDS);
    rtc_time->minute = rtc_read_register(RTC_MINUTES);
    rtc_time->hour = rtc_read_register(RTC_HOURS);
    rtc_time->weekday = rtc_read_register(RTC_WEEKDAY);
    rtc_time->day_of_month = rtc_read_register(RTC_DAY_OF_MONTH);
    rtc_time->month = rtc_read_register(RTC_MONTH);
    rtc_time->year = rtc_read_register(RTC_YEAR);
    rtc_time->century = rtc_read_register(RTC_CENTURY);

    do {
        last_second = rtc_time->second;
        last_minute = rtc_time->minute;
        last_hour = rtc_time->hour;
        last_weekday = rtc_time->weekday;
        last_day_of_month = rtc_time->day_of_month;
        last_month = rtc_time->month;
        last_year = rtc_time->year;
        last_century = rtc_time->century;

        while (rtc_update_in_progress_flag() != 0) {}

        rtc_time->second = rtc_read_register(RTC_SECONDS);
        rtc_time->minute = rtc_read_register(RTC_MINUTES);
        rtc_time->hour = rtc_read_register(RTC_HOURS);
        rtc_time->weekday = rtc_read_register(RTC_WEEKDAY);
        rtc_time->day_of_month = rtc_read_register(RTC_DAY_OF_MONTH);
        rtc_time->month = rtc_read_register(RTC_MONTH);
        rtc_time->year = rtc_read_register(RTC_YEAR);
        rtc_time->century = rtc_read_register(RTC_CENTURY);

    } while ((last_second != rtc_time->second) || (last_minute != rtc_time->minute) ||
             (last_hour != rtc_time->hour) || (last_weekday != rtc_time->weekday) ||
             (last_day_of_month != rtc_time->day_of_month) ||
             (last_month != rtc_time->month) || (last_year != rtc_time->year) ||
             (last_century != rtc_time->century));

    uint8_t register_B = rtc_read_register(RTC_STATUS_REGISTER_B);
    uint8_t bcd = register_B >> 2 ? false : true;

    if (bcd) {
        rtc_time->second = rtc_bcd_to_bin(rtc_time->second);
        rtc_time->minute = rtc_bcd_to_bin(rtc_time->minute);
        rtc_time->hour = rtc_bcd_to_bin(rtc_time->hour);
        rtc_time->weekday = rtc_bcd_to_bin(rtc_time->weekday);
        rtc_time->day_of_month = rtc_bcd_to_bin(rtc_time->day_of_month);
        rtc_time->month = rtc_bcd_to_bin(rtc_time->month);
        rtc_time->year = rtc_bcd_to_bin(rtc_time->year);
        rtc_time->century = rtc_bcd_to_bin(rtc_time->century);
    }

    if (!(register_B >> 1) && (rtc_time->hour & 0x80)) {
        rtc_time->hour = ((rtc_time->hour & 0x7F) + 12) % 24;
    }

    rtc_time->year += rtc_time->century * 100;
}

void rtc_debug_print() {
    datetime_t rtc_time;

    for (;;) {
        rtc_read_time(&rtc_time);
        serial_printf("%02i:%02i:%02i - %02i.%02i.%04i\n", rtc_time.hour,
                      rtc_time.minute, rtc_time.second, rtc_time.day_of_month, rtc_time.month,
                      rtc_time.year);
        timer_sleep_ticks(10);
    }
}