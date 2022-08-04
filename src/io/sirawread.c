#include "PR/os_internal.h"
#include "siint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSiRawReadIo(u32 devAddr, u32* data) {
    if (__osSiDeviceBusy()) {
        return -1;
    }
    
    *data = IO_READ(devAddr);
    return 0;
}
