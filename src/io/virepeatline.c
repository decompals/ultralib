#include "PR/os_internal.h"
#include "viint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void osViRepeatLine(u8 active) {
    register u32 saveMask = __osDisableInt();

    if (active) {
        __osViNext->state |= VI_STATE_REPEATLINE;
    } else {
        __osViNext->state &= ~VI_STATE_REPEATLINE;
    }

    __osRestoreInt(saveMask);
}
