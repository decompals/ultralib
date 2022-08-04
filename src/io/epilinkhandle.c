#include "piint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osEPiLinkHandle(OSPiHandle* EPiHandle)
{
    register s32 saveMask = __osDisableInt();

    EPiHandle->next = __osPiTable;
    __osPiTable = EPiHandle;

    __osRestoreInt(saveMask);
    return 0;
}
