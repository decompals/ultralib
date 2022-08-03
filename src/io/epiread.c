#include "piint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 __osEPiRawReadIo(OSPiHandle*, u32, u32*);

s32 osEPiReadIo(OSPiHandle* pihandle, u32 devAddr, u32* data) {
    register s32 ret;

    __osPiGetAccess();
    ret = __osEPiRawReadIo(pihandle, devAddr, data);
    __osPiRelAccess();

    return ret;
}
