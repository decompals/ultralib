#include "PR/os_internal.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

u32 __osSpGetStatus() {
    return IO_READ(SP_STATUS_REG);
}
