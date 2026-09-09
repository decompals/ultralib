#include "PR/os_version.h"
#include "PR/ultraerror.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#endif

extern OSErrorHandler __osErrorHandler;

OSErrorHandler osSetErrorHandler(OSErrorHandler handler) {
    OSErrorHandler oldHandler = __osErrorHandler;

    __osErrorHandler = handler;
    return oldHandler;
}
