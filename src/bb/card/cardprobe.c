#include "PR/os_internal.h"
#include "PR/bcp.h"
#include "PR/bbcard.h"

void __osBbDelay(u32);

s32 osBbCardProbe(u32 dev) {
    s32 count;
    s32 rv;
    u8 save = __osBbCardChange;

    __osBbCardChange = FALSE;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    IO_WRITE(PI_NAND_DATA_BUFFER(0, 0), 0);
    IO_WRITE(PI_48_REG, NAND_READ_STATUS(0, dev, FALSE));

    rv = BBCARD_ERR_NO_CARD;

    for (count = 0; count < 1000; count++) {
        if (!(IO_READ(PI_48_REG) & NAND_STATUS_BUSY)) {
            if ((IO_READ(PI_NAND_DATA_BUFFER(0, 0)) >> 0x18) == 0xC0) {
                rv = 0;

#ifdef _DEBUG
                osSyncPrintf("probe succeeds\n");
#endif
                __osBbCardFlushEvent();
                goto ret;
            }
            break;
        }
        __osBbDelay(10);
    }
    IO_WRITE(PI_48_REG, NAND_CTRL_CLR_INTR);
    __osBbCardChange = save;
#ifdef _DEBUG
    osSyncPrintf("probe fails\n");
#endif

ret:
    __osBbCardRelAccess();
    return rv;
}
