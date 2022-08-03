#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "../os/osint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSpRawWriteIo(u32 devAddr, u32 data) {
    if (__osSpDeviceBusy()) {
        return -1;
    }

    IO_WRITE(devAddr, data);
    return 0;
}
