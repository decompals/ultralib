#include <PR/os_internal.h>
#include "viint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

u32 osViGetCurrentMode(void) {
    register u32 savedMask = __osDisableInt();
    register u32 modeType = __osViCurr->modep->type;

    __osRestoreInt(savedMask);
    return modeType;
}
