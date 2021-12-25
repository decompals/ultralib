#include "PR/os_internal.h"

void __osResetGlobalIntMask(OSHWIntr mask) {
    register u32 saveMask = __osDisableInt();

    // TODO magic number
    __OSGlobalIntMask &= ~(mask & ~0x401);

    __osRestoreInt(saveMask);
}
