#include "PR/os_internal.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 osPiGetDeviceType(void) {
    return osRomType;
}
