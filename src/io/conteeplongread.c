#include "PR/os_internal.h"
#include "controller.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osEepromLongRead(OSMesgQueue* mq, u8 address, u8* buffer, int length) {
    s32 ret = 0;

    while (length > 0) {
        ERRCK(osEepromRead(mq, address, buffer));
        length -= EEPROM_BLOCK_SIZE;
        address++;
        buffer += EEPROM_BLOCK_SIZE;
    }

    return ret;
}
