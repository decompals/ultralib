#include "PR/os_internal.h"
#include "PR/os_bb_export.h"
#include "bcp.h"
#include "macros.h"

#define RTC_MASK ((PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_HI) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI))

#define RTC_ADDR 0xD0
#define RTC_WR   0
#define RTC_RD   1

void __osBbDelay(u32 usec);

static void write_rtc(u32 x) {
    IO_WRITE(PI_GPIO_REG, x);
    __osBbDelay(2);
}

static void send_start(u8 write) {
    u32 i;
    u32 j;
    u32 mask = IO_READ(PI_GPIO_REG) & ~RTC_MASK;
    u8 byte[2];

    byte[0] = (!write) ? (RTC_ADDR | RTC_RD) : (RTC_ADDR | RTC_WR);
    byte[1] = 0;

    // Send start signal (DAT HIGH -> LOW while CLK HIGH) assuming both CLK and DAT were initially HIGH
    // Cyc -1 | 0 | 1
    // CLK ‾‾‾|‾‾‾|___
    // DAT ‾‾‾|___|___
    write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
    write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));

    for (i = 0; i < write + 1; i++) {
        // Send address in byte[0], for writes also send word address in byte[1]
        for (j = 0; j < 8; j++) {
            u32 b = ((byte[i] >> (7 - j)) & 1) ? PI_GPIO_RTC_DAT_HI : PI_GPIO_RTC_DAT_LO;

            // Transmit single bit to the RTC
            // Cyc  0 | 1 | 2
            // CLK ___|‾‾‾|___
            // DAT  b | b | b
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
        }

        // Toggle CLK to receive ACK from the RTC, but don't read it
        // Cyc  0 | 1
        // CLK ___|‾‾‾
        // DAT  x | x
        write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
        write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
    }

    // End on CLK LOW
    // Cyc  0
    // CLK ___
    // DAT  x
    write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
}

static void send_stop(void) {
    u32 mask = IO_READ(PI_GPIO_REG) & ~RTC_MASK;

    // Send stop signal (DAT LOW -> HIGH while CLK HIGH)
    // Cyc  0 | 1 | 2
    // CLK ___|‾‾‾|‾‾‾
    // DAT ___|___|‾‾‾
    write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
    write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
    write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_HI) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
}

static void read_bytes(u8* bytes, u8 len) {
    u32 ack;
    u32 i;
    u32 mask = IO_READ(PI_GPIO_REG) & ~RTC_MASK;

    while (len-- > 0) {
        u32 x = 0;

        // Read 1 byte
        for (i = 0; i < 8; i++) {
            // Toggle CLK to receive the bit from the RTC
            // Cyc  0 | 1
            // CLK ___|‾‾‾
            // DAT  x | x
            write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
            write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));

            // Read the bit sent by the RTC
            x <<= 1;
            x |= PI_GPIO_GET_RTC_DAT(IO_READ(PI_GPIO_REG));
        }
        *(bytes++) = x;

        ack = (len == 0) ? (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_HI) : (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO);

        // Send ACK or NACK, DAT HIGH is NACK while DAT LOW is ACK. NACK is sent at th end.
        // Cyc  0 | 1
        // CLK ___|‾‾‾
        // DAT ___| a
        write_rtc(mask | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_LO) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
        write_rtc(mask | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI) | ack);
    }
    send_stop();
}

static void write_bytes(u8* bytes, u8 len) {
    u32 i;
    u32 mask = IO_READ(PI_GPIO_REG) & ~RTC_MASK;

    while (len-- > 0) {
        u32 x = *(bytes++);

        for (i = 0; i < 8; i++) {
            u32 b = (x & 0x80) ? PI_GPIO_RTC_DAT_HI : PI_GPIO_RTC_DAT_LO;

            // Transmit single bit to the RTC
            // Cyc  0 | 1 | 2
            // CLK ___|‾‾‾|‾‾‾
            // DAT  b | b | b
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
            write_rtc(mask | (PI_GPIO_O_RTC_DAT | b) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
            x <<= 1;
        }

        // Toggle CLK to receive ACK from the RTC, read but don't check?
        // Cyc  0 | 1 | 2 | 3
        // CLK ___|‾‾‾|‾‾‾|___
        // DAT  x | x | x | x
        write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
        write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
        IO_READ(PI_GPIO_REG);
        write_rtc(mask | PI_GPIO_I_RTC_DAT | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_LO));
    }
    send_stop();
}

void osBbRtcInit(void) {
    // Set line state to idle (both CLK and DAT HIGH)
    // Cyc  0
    // CLK ‾‾‾
    // DAT ‾‾‾
    write_rtc(IO_READ(PI_GPIO_REG) | (PI_GPIO_O_RTC_DAT | PI_GPIO_RTC_DAT_HI) | (PI_GPIO_O_RTC_CLK | PI_GPIO_RTC_CLK_HI));
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
