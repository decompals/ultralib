#include "PR/os_internal.h"
#include "bcp.h"
#include "macros.h"

void __osBbDelay(u32 usec);

static void write_rtc(u32 x) {
    IO_WRITE(PI_GPIO_REG, x);
    __osBbDelay(2);
}

static void send_start(u8 write) {
    u32 i;
    u32 j;
    u32 mask = IO_READ(PI_GPIO_REG) & ~(PI_GPIO_MASK_RTC_0 | PI_GPIO_MASK_RTC_1 | 0xC);
    u8 byte[2];

    byte[0] = (!write) ? 0xD1 : 0xD0;
    byte[1] = 0;

    write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 4));
    write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 0));

    for (i = 0; i < write + 1; i++) {
        for (j = 0; j < 8; j++) {
            u32 b = ((byte[i] >> (7 - j)) & 1) ? (2 << 2) : (0 << 2);

            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 0));
            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 4));
            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 0));
        }
        write_rtc(mask | 0x40);
        write_rtc(mask | 0x44);
    }
    write_rtc(mask | 0x40);
}

static void send_stop(void) {
    u32 mask = IO_READ(PI_GPIO_REG) & ~(PI_GPIO_MASK_RTC_1 | PI_GPIO_MASK_RTC_0 | 0xC);
    write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 0x0));
    write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 0x4));
    write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 0xC));
}

static void read_bytes(u8* bytes, u8 len) {
    u32 ack;
    u32 i;
    u32 mask = IO_READ(PI_GPIO_REG) & ~(PI_GPIO_MASK_RTC_0 | PI_GPIO_MASK_RTC_1 | 0xC);

    while (len-- > 0) {
        u32 x = 0;

        for (i = 0; i < 8; i++) {
            write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 0));
            write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 4));
            x <<= 1;
            x |= (IO_READ(PI_GPIO_REG) >> 3) & 1;
        }
        *(bytes++) = x;

        ack = (len == 0) ? (PI_GPIO_MASK_RTC_1 | 8) : (PI_GPIO_MASK_RTC_1 | 0);

        write_rtc(mask | (PI_GPIO_MASK_RTC_1 | 0) | (PI_GPIO_MASK_RTC_0 | 0));
        write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 4) | ack);
    }
    send_stop();
}

static void write_bytes(u8* bytes, u8 len) {
    u32 i;
    u32 mask = IO_READ(PI_GPIO_REG) & ~(PI_GPIO_MASK_RTC_0 | PI_GPIO_MASK_RTC_1 | 0xC);

    while (len-- > 0) {
        u32 x = *(bytes++);

        for (i = 0; i < 8; i++) {
            u32 b = (x & 0x80) ? 8 : 0;

            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 0));
            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 4));
            write_rtc(mask | (PI_GPIO_MASK_RTC_1 | b) | (PI_GPIO_MASK_RTC_0 | 4));
            x <<= 1;
        }
        write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 0));
        write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 4));
        IO_READ(PI_GPIO_REG);
        write_rtc(mask | (PI_GPIO_MASK_RTC_0 | 0));
    }
    send_stop();
}

void osBbRtcInit(void) {
    write_rtc(IO_READ(PI_GPIO_REG) | PI_GPIO_MASK_RTC_0 | PI_GPIO_MASK_RTC_1 | 0xC);
}

void osBbRtcSet(u8 year, u8 month, u8 day, u8 dow, u8 hour, u8 min, u8 sec) {
    u8 data[8];

    data[7] = 0;
    data[6] = ((year / 10) << 4) | (year % 10);
    data[5] = ((month / 10) << 4) | (month % 10);
    data[4] = ((day / 10) << 4) | (day % 10);
    data[3] = ((dow / 10) << 4) | (dow % 10);
    data[2] = ((hour / 10) << 4) | (hour % 10);
    data[1] = ((min / 10) << 4) | (min % 10);
    data[0] = ((sec / 10) << 4) | (sec % 10);
    send_start(TRUE);
    write_bytes(data, ARRLEN(data));
}

void osBbRtcGet(u8* year, u8* month, u8* day, u8* dow, u8* hour, u8* min, u8* sec) {
    u8 data[8];

    send_start(TRUE);
    send_start(FALSE);
    read_bytes(data, ARRLEN(data));
    *year = ((data[6] >> 4) * 10) + (data[6] & 0xF);
    *month = ((data[5] >> 4) * 10) + (data[5] & 0xF);
    *day = ((data[4] >> 4) * 10) + (data[4] & 0xF);
    *dow = ((data[3] >> 4) * 10) + (data[3] & 0xF);
    *hour = ((data[2] >> 4) * 10) + (data[2] & 0xF);
    *min = ((data[1] >> 4) * 10) + (data[1] & 0xF);
    *sec = ((data[0] >> 4) * 10) + (data[0] & 0xF);
}
