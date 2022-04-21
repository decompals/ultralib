#include "PR/os_internal.h"
#include "rcp.h"

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

    IO_WRITE(PI_BASE_REG + 0x70, 0); // PI_CARD_BLK_OFFSET_REG
    IO_WRITE(PI_BASE_REG + 0x48, (dev << 12) | 0xD1900004); // PI_CARD_CNT_REG

    rv = __osBbCardWaitEvent();
    status = IO_READ(PI_BASE_REG + 0x10000); // PI_EX_DMA_BUF
    *mfg = status >> 0x18;
    *type = (status >> 0x10) & 0xFF;
    __osBbCardMultiplane = (status & 0xFF) == 0xC0;

    __osBbCardRelAccess();
    return rv;
}
