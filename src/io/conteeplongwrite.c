#include "PR/os_internal.h"
#include "PRinternal/controller.h"

#if BUILD_VERSION < VERSION_I
#define EEPROM_ACCESS_DELAY_US 12000
#endif

s32 osEepromLongWrite(OSMesgQueue* mq, u8 address, u8* buffer, int length) {
    s32 ret = 0;

#if BUILD_VERSION < VERSION_I
    if (address > EEPROM_MAXBLOCKS) {
        return -1;
    }
#endif

    while (length > 0) {
        ERRCK(osEepromWrite(mq, address, buffer));
        length -= EEPROM_BLOCK_SIZE;
        address++;
        buffer += EEPROM_BLOCK_SIZE;
#if BUILD_VERSION < VERSION_I
        osSetTimer(&__osEepromTimer, EEPROM_ACCESS_DELAY_US * osClockRate / 1000000, 0, &__osEepromTimerQ, &__osEepromTimerMsg);
#else
        osSetTimer(&__osEepromTimer, OS_USEC_TO_CYCLES(12000), 0, &__osEepromTimerQ, &__osEepromTimerMsg);
#endif
        osRecvMesg(&__osEepromTimerQ, NULL, OS_MESG_BLOCK);
    }

    return ret;
}
