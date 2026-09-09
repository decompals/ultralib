#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif

void osExit(void) {
    __osGIOInterrupt(16);

    for (;;) {
        ;
    }
}
