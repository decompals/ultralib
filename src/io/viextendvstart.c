#include "PR/os_internal.h"
#include "PR/ultraerror.h"
#include "PRinternal/viint.h"

#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
extern u32 __additional_scanline;
#endif

void osViExtendVStart(u32 value) {
#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(134, 0);
        return;
    }

    if (value > 48) {
        __osError(135, 1, value);
        return;
    }
#endif
    __additional_scanline = value;
    return;
}
