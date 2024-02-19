#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "../os/osint.h"
#include "assert.h"



































// Adjust line numbers to match assert
#if BUILD_VERSION < VERSION_J
#line 41
#endif

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSpRawWriteIo(u32 devAddr, u32 data) {
    assert((devAddr & 0x3) == 0);
    if (__osSpDeviceBusy()) {
        return -1;
    }

    IO_WRITE(devAddr, data);
    return 0;
}
