#include "ultra64.h"
#include "PR/os_internal_flash.h"


extern u32 __osBbFlashAddress;
extern u32 __osBbFlashSize;

s32 osFlashAllErase(void) {
#ifdef BBPLAYER
    if (__osBbFlashSize != 0) {
        int i;
        u32* p = (u32*)__osBbFlashAddress;

        for (i = 0; i < __osBbFlashSize / 4; i++) {
            p[i] = -1;
        }
        return FLASH_STATUS_ERASE_OK;
    }
    return FLASH_STATUS_ERASE_ERROR;
#else
    u32 status;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;
    OSMesg dummy;

    // start chip erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_CHIP_ERASE);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    // wait for completion by polling erase-busy flag
    osCreateMesgQueue(&timerMesgQueue, &dummy, 1);
    do {
        osSetTimer(&mytimer, OS_USEC_TO_CYCLES(15000), 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    // check erase operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    // check for success
    if (((status & 0xFF) == 8) || ((status & 0xFF) == 0x48) || ((status & 8) == 8)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
#endif
}
