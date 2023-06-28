#include "PR/os_internal.h"
#include "../io/piint.h"

#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void __osGIOInterrupt(s32 value) {
    __osPiGetAccess();
    __osGIORawInterrupt(value);
    __osPiRelAccess();
}
