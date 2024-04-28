#include "PR/os_internal.h"
#include "PR/os_bbcard.h"
#include "PR/bcp.h"

s32 __osBbCardGetAccess(void);
void __osBbCardRelAccess(void);

s32 __osBbCardWaitEvent(void);

extern s8 __osBbCardMultiplane;

s32 osBbCardReadId(u32 dev, u32* mfg, u32* type) {
    u32 status;
    s32 rv;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    IO_WRITE(PI_70_REG, 0);
    IO_WRITE(PI_48_REG, (dev << 12) | 0xD1900004);

    rv = __osBbCardWaitEvent();
    status = IO_READ(PI_10000_REG(0));
    *mfg = status >> 0x18;
    *type = (status >> 0x10) & 0xFF;
    __osBbCardMultiplane = (status & 0xFF) == 0xC0;

    __osBbCardRelAccess();
    return rv;
}
