#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif
#include "PRinternal/viint.h"

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

void osViRepeatLine(u8 active) {
    register u32 saveMask = __osDisableInt();

    if (active) {
        __osViNext->state |= VI_STATE_REPEATLINE;
    } else {
        __osViNext->state &= ~VI_STATE_REPEATLINE;
    }

    __osRestoreInt(saveMask);
}
