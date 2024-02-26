#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "../os/osint.h"
#include "assert.h"


// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSpRawReadIo(u32 devAddr, u32* data) {
#if BUILD_VERSION < VERSION_J
#line 48
#else
#line 51
#endif
    assert((devAddr & 0x3) == 0);
    assert(data != NULL);

    if (__osSpDeviceBusy()) {
        return -1;
    }

    *data = IO_READ(devAddr);
    return 0;
}
