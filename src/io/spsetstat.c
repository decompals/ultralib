#include "PR/os_internal.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void __osSpSetStatus(u32 data) {
    IO_WRITE(SP_STATUS_REG, data);
}
