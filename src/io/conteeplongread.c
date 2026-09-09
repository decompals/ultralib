#include "PR/os_internal.h"
#include "PRinternal/controller.h"

#if BUILD_VERSION < VERSION_I
#define EEPROM_ACCESS_DELAY_US 12000
#endif

s32 osEepromLongRead(OSMesgQueue* mq, u8 address, u8* buffer, int length) {
    s32 ret = 0;

#if BUILD_VERSION < VERSION_I
    if (address > EEPROM_MAXBLOCKS) {
        return -1;
    }
#endif

    while (length > 0) {
        ERRCK(osEepromRead(mq, address, buffer));
        length -= EEPROM_BLOCK_SIZE;
        address++;
        buffer += EEPROM_BLOCK_SIZE;
#if BUILD_VERSION < VERSION_I
        osSetTimer(&__osEepromTimer, EEPROM_ACCESS_DELAY_US * osClockRate / 1000000, 0, &__osEepromTimerQ, &__osEepromTimerMsg);
        osRecvMesg(&__osEepromTimerQ, NULL, OS_MESG_BLOCK);
#endif
    }

    return ret;
}
