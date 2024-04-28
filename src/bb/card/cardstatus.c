#include "PR/os_internal.h"
#include "PR/bcp.h"
#include "PR/bbcard.h"

s32 osBbCardUnhappy(void) {
    if (!__osBbCardInit || !__osBbCardPresent()) {
        return BBCARD_ERR_NO_CARD;
    }
    if (__osBbCardChange) {
        return BBCARD_ERR_CHANGED;
    }
    return 0;
}

s32 __osBbCardStatus(u32 dev, u8* status, u32 buf) {
    s32 rv;
    u32 cmd;

    cmd = __osBbCardMultiplane ? NAND_CMD_READ_STATUS_MP : NAND_CMD_READ_STATUS;
    IO_WRITE(PI_48_REG, NAND_READ_STATUS_CMD(cmd, buf, dev, TRUE));

    rv = __osBbCardWaitEvent();
    if (rv == 0) {
        *status = IO_READ(PI_NAND_DATA_BUFFER(buf != 0, 0)) >> 0x18;
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
