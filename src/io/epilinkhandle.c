#include "piint.h"

s32 osEPiLinkHandle(OSPiHandle *pihandle)
{
    register s32 mask = __osDisableInt();

    pihandle->next = __osPiTable;
    __osPiTable = pihandle;

    __osRestoreInt(mask);
    return 0;
}
