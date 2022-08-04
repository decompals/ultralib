#include "ultra64.h"
#include "PR/os_internal_flash.h"

extern u32 __osBbFlashAddress;
extern u32 __osBbFlashSize;

void osFlashAllEraseThrough(void) {
#ifdef BBPLAYER
    if (__osBbFlashSize != 0) {
        int i;
        u32* p = (u32*)__osBbFlashAddress;

        for (i = 0; i < __osBbFlashSize / 4; i++) {
            p[i] = -1;
        }
    }
#else
    // start chip erase operation, no waiting for completion
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_CHIP_ERASE);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);
#endif
}
