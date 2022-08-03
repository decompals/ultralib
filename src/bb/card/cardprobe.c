#include "PR/os_internal.h"
#include "PR/rcp.h"

s32 __osBbCardFlushEvent(void);
s32 __osBbCardGetAccess(void);
void __osBbCardRelAccess(void);
void __osBbDelay(u32);
extern u8 __osBbCardChange;

s32 osBbCardProbe(u32 dev) {
    s32 count;
    s32 rv;
    u8 save = __osBbCardChange;

    __osBbCardChange = FALSE;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    IO_WRITE(PI_BASE_REG + 0x10000, 0);
    IO_WRITE(PI_BASE_REG + 0x48, (dev << 0xC) | 0x90700001);

    rv = -1;

    for (count = 0; count < 1000; count++) {
        if (!(IO_READ(PI_BASE_REG + 0x48) & 0x80000000)) {
            if ((IO_READ(PI_BASE_REG + 0x10000) >> 0x18) == 0xC0) {
                rv = 0;

#ifndef NDEBUG
                osSyncPrintf("probe succeeds\n");
#endif
                __osBbCardFlushEvent();
                goto ret;
            }
            break;
        }
        __osBbDelay(10);
    }
    IO_WRITE(PI_BASE_REG + 0x48, 0);
    __osBbCardChange = save;
#ifndef NDEBUG
    osSyncPrintf("probe fails\n");
#endif

ret:
    __osBbCardRelAccess();
    return rv;
}
