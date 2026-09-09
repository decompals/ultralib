#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif

void __osResetGlobalIntMask(OSHWIntr mask) {
    register u32 saveMask = __osDisableInt();

    __OSGlobalIntMask &= ~(mask & ~OS_IM_RCP);

    __osRestoreInt(saveMask);
}
