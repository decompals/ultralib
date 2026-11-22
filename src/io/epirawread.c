#include "PRinternal/piint.h"
#include "PR/ultraerror.h"
#include "assert.h"

// Adjust line numbers to match assert
#if BUILD_VERSION <= VERSION_H
#line 22
#elif BUILD_VERSION < VERSION_J
#line 26
#else
#line 27
#endif

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

s32 __osEPiRawReadIo(OSPiHandle* pihandle, u32 devAddr, u32* data) {
    register u32 stat;
    register u32 domain;

#ifdef _DEBUG
    if (devAddr & 0x3) {
        __osError(ERR_OSPIRAWREADIO, 1, devAddr);
        return -1;
    }
#endif
    assert(data != NULL);

#if BUILD_VERSION <= VERSION_H
    WAIT_ON_IOBUSY(stat);
#else
    EPI_SYNC(pihandle, stat, domain);
#endif
    *data = IO_READ(pihandle->baseAddress | devAddr);

    return 0;
}
