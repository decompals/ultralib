#include "piint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osPiRawReadIo(u32 devAddr, u32* data) {
    register u32 stat;

    WAIT_ON_IOBUSY(stat);
    *data = IO_READ((u32)osRomBase | devAddr);

    return 0;
}
