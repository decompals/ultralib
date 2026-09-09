#include "PR/os_internal.h"
#include "PRinternal/piint.h"

#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
extern OSDevMgr __osPiDevMgr;
#endif

OSMesgQueue* osPiGetCmdQueue(void) {
    if (!__osPiDevMgr.active) {
        return NULL;
    } else {
        return __osPiDevMgr.cmdQueue;
    }
}
