#include "PR/os_internal.h"
#include "PR/ultraerror.h"
#include "assert.h"
#include "viint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void osViSwapBuffer(void* frameBufPtr) {
    u32 saveMask;

#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(ERR_OSVISWAPBUFFER_VIMGR, 0);
        return;
    }

#line 62
    assert(frameBufPtr != NULL);

    if ((u32)frameBufPtr & 0x3f) {
        __osError(ERR_OSVISWAPBUFFER_ADDR, 1, frameBufPtr);
        return;
    }
#endif

    saveMask = __osDisableInt();

    __osViNext->framep = frameBufPtr;
    __osViNext->state |= VI_STATE_BUFFER_UPDATED;
    __osRestoreInt(saveMask);
}
