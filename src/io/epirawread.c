#include "PRinternal/piint.h"
#include "PR/ultraerror.h"
#include "assert.h"




















// Adjust line numbers to match assert
#if BUILD_VERSION >= VERSION_J
#line 23
#elif BUILD_VERSION >= VERSION_I
#line 26
#else
#line 26
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

#if BUILD_VERSION >= VERSION_I
    EPI_SYNC(pihandle, stat, domain);
#else
    WAIT_ON_IOBUSY(stat);
#endif
    *data = IO_READ(pihandle->baseAddress | devAddr);

    return 0;
}
