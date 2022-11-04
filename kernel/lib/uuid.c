#include "uuid.h"
#include <drivers/pit.h>
#include <drivers/rtc.h>
#include <printf.h>
// really crappy implementation of uuid generation
void uuid_generate_random(uuid_t uuid_buffer) {
    datetime_t datetime; // 9 bytes
    rtc_read_time(&datetime);
    uint8_t datetime_bytes[9] = {datetime.second,       datetime.minute, datetime.hour,
                                 datetime.day_of_month, datetime.month,  datetime.year,
                                 datetime.century,      datetime.weekday};
    uint32_t tmp = pit_tick_get();
    uint8_t pit[4] = {(uint8_t)(tmp >> 24), (uint8_t)(tmp >> 16), (uint8_t)(tmp >> 8),
                      (uint8_t)(tmp)};
    uint16_t* ptr1 = (uint16_t*)datetime_bytes;
    uint8_t* ptr2 = (uint16_t*)pit;
    uuid_buffer[0] = datetime_bytes[0];
    uuid_buffer[1] = datetime_bytes[1];
    uuid_buffer[2] = datetime_bytes[2];
    uuid_buffer[3] = datetime_bytes[3];
    uuid_buffer[4] = datetime_bytes[4];
    uuid_buffer[5] = datetime_bytes[5];
    uuid_buffer[6] = datetime_bytes[6];
    uuid_buffer[7] = datetime_bytes[7];
    uuid_buffer[8] = datetime_bytes[8];
    uuid_buffer[9] = pit[0];
    uuid_buffer[10] = pit[1];
    uuid_buffer[11] = pit[2];
    uuid_buffer[12] = pit[3];
    uuid_buffer[13] = ptr1[0];
    uuid_buffer[14] = ptr1[1];
    uuid_buffer[15] = ptr2[0];
}

// generate a 36 chars long uuid string from a uuid_t 
void uuid_unparse_lower(uuid_t uuid_buffer, char* uuid_string) {
    for (int i = 0; i < 16; i++) {
        sprintf(uuid_string + i * 2, "%02x", uuid_buffer[i]);
    }
    uuid_string[37] = '\0';
}
