#include "PR/os_internal.h"

void osFlashClearStatus(void) {
#ifndef BBPLAYER
    // select status mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    // clear status
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress, 0);
#endif
    return;
}
