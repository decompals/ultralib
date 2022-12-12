#include "PR/os_internal.h"
#include "PR/bcp.h"
#include "../../io/viint.h"

void __osBbDelay(u32);

void __osBbVideoPllInit(s32 tvType) {
    u32 avctrl;
    register u32 mask;

    mask = __osDisableInt();

    IO_WRITE(VI_CONTROL_REG, 0);
    IO_WRITE(AI_CONTROL_REG, 0);

    __osBbDelay(50);

    avctrl = IO_READ(MI_30_REG);
    avctrl &= ~0x2000000;
    IO_WRITE(MI_30_REG, avctrl);

    __osBbDelay(50);

    IO_WRITE(MI_30_REG, avctrl | 1);

    __osBbDelay(50);

    if (tvType == OS_TV_TYPE_PAL) {
        avctrl = 0x93644;
    } else if (tvType == OS_TV_TYPE_MPAL) {
        avctrl = 0x294244;
    } else {
        avctrl = 0x194244;
    }

    IO_WRITE(MI_30_REG, avctrl | 0x800000 | 1);
    IO_READ(MI_30_REG);

    __osBbDelay(2);

    IO_WRITE(MI_30_REG, avctrl);
    IO_READ(MI_30_REG);

    __osBbDelay(1000);

    IO_WRITE(MI_30_REG, avctrl | 0x2000000);
    IO_READ(MI_30_REG);

    __osRestoreInt(mask);
}
