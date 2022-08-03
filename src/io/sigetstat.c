#include "PR/os_internal.h"
#include "siint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

u32 __osSiGetStatus() {
    return IO_READ(SI_STATUS_REG);
}
