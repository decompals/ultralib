#include "PR/os_internal.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

u32 osDpGetStatus() {
    return IO_READ(DPC_STATUS_REG);
}
