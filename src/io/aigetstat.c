#include "PR/rcp.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

u32 osAiGetStatus() {
    return IO_READ(AI_STATUS_REG);
}
