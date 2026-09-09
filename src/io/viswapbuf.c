#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif
#include "PR/ultraerror.h"
#include "assert.h"
#include "PRinternal/viint.h"













































// TODO: this comes from a header
#ident "$Revision: 1.17 $"

void osViSwapBuffer(void* frameBufPtr) {
    u32 saveMask;

#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(ERR_OSVISWAPBUFFER_VIMGR, 0);
        return;
    }

#if BUILD_VERSION <= VERSION_G
#line 61 "../io/viswapbuf.c"
#endif
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
