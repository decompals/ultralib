#include "PR/os_internal.h"
#include "PR/piint.h"

void __osGIOInterrupt(s32 value) {
    __osPiGetAccess();
    __osGIORawInterrupt(value);
    __osPiRelAccess();
}
