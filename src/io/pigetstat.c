#include "PR/os_internal.h"
#include "piint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

u32 osPiGetStatus() {
    return IO_READ(PI_STATUS_REG);
}
