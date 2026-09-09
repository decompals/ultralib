#include "PR/os_internal.h"
#include "PR/rcp.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "PRinternal/viint.h"
#endif

u32 osViGetCurrentLine(void) {
    return IO_READ(VI_CURRENT_REG);
}
