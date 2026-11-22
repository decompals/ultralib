#include "PRinternal/piint.h"
#include "PR/ultraerror.h"

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

s32 __osEPiRawWriteIo(OSPiHandle* pihandle, u32 devAddr, u32 data) {
    register u32 stat;
    register u32 domain;

#ifdef _DEBUG
    if (devAddr & 0x3) {
        __osError(ERR_OSPIRAWWRITEIO, 1, devAddr);
        return -1;
    }
#endif

#if BUILD_VERSION >= VERSION_I
    EPI_SYNC(pihandle, stat, domain);
#else
    WAIT_ON_IOBUSY(stat);
#endif
    IO_WRITE(pihandle->baseAddress | devAddr, data);

    return 0;
}
