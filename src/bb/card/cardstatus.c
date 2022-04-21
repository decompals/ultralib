#include "PR/os_internal.h"
#include "PR/rcp.h"

s32 __osBbCardWaitEvent(void);

s32 __osBbCardGetAccess(void);
void __osBbCardRelAccess(void);

s32 __osBbCardPresent(void);

extern u16 __osBbCardBlocks;
extern u8 __osBbCardChange;
extern u8 __osBbCardInit;
extern u8 __osBbCardMultiplane;

s32 osBbCardUnhappy(void) {
    if (!__osBbCardInit || !__osBbCardPresent()) {
        return -1;
    }
    if (__osBbCardChange) {
        return -4;
    }
    return 0;
}

s32 __osBbCardStatus(u32 dev, u8* status, u32 buf) {
    s32 rv;
    u32 cmd;

    cmd = __osBbCardMultiplane ? 0x71 : 0x70;

    IO_WRITE(PI_BASE_REG + 0x48, 0xD0000000 | (cmd << 0x10) | (buf << 0xE) | (dev << 0xC) | 1);

    rv = __osBbCardWaitEvent();
    if (rv == 0) {
        *status = IO_READ(PI_BASE_REG + 0x10000 + ((buf != 0) ? 0x200 : 0x000)) >> 0x18;
    } else {
        *status = 0;
    }
    return rv;
}

s32 osBbCardStatus(u32 dev, u8* status) {
    s32 rv;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = __osBbCardStatus(dev, status, 0);

    __osBbCardRelAccess();
    return rv;
}

s32 osBbCardChange(void) {
    return __osBbCardChange;
}

s32 osBbCardClearChange(void) {
    __osBbCardChange = FALSE;
    return __osBbCardPresent();
}

u32 osBbCardBlocks(u32 dev) {
    s32 rv;
    
    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = __osBbCardBlocks;

    __osBbCardRelAccess();
    return rv;
}
