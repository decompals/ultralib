#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

s32 osPiGetDeviceType(void) {
    return osRomType;
}
