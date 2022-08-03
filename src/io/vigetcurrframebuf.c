#include <PR/os_internal.h>
#include "viint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void *osViGetCurrentFramebuffer(void) {
    register u32 saveMask = __osDisableInt();
    void *framep = __osViCurr->framep;

    __osRestoreInt(saveMask);
    return framep;
}
