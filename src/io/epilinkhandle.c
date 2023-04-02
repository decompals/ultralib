#include "piint.h"

s32 osEPiLinkHandle(OSPiHandle* EPiHandle) {
    register s32 saveMask = __osDisableInt();

    EPiHandle->next = __osPiTable;
    __osPiTable = EPiHandle;

    __osRestoreInt(saveMask);
    return 0;
}
