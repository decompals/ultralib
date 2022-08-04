#include <PR/os_internal.h>
#include "viint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void osViSwapBuffer(void* frameBufPtr) {
    u32 saveMask = __osDisableInt();

    __osViNext->framep = frameBufPtr;
    __osViNext->state |= VI_STATE_BUFFER_UPDATED;
    __osRestoreInt(saveMask);
}
