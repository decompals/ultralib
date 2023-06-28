#include "PR/os_internal.h"
#include "assert.h"
#include "siint.h"

// Adjust line numbers to match assert
#if BUILD_VERSION < VERSION_J
#line 45
#endif

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSiRawReadIo(u32 devAddr, u32* data) {
#ifdef _DEBUG
#line 53
    assert((devAddr & 0x3) == 0);
    assert(data != NULL);
#endif

    if (__osSiDeviceBusy()) {
        return -1;
    }

    *data = IO_READ(devAddr);
    return 0;
}
