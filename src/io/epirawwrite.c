#include "piint.h"

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

s32 __osEPiRawWriteIo(OSPiHandle* pihandle, u32 devAddr, u32 data) {
    s32 stat;

    EPI_SYNC(pihandle, stat);
    IO_WRITE(pihandle->baseAddress | devAddr, data);

    return 0;
}
