#include "PR/os_internal.h"
#include "PR/bcp.h"
#include "PR/bbcard.h"

s32 osBbCardReadId(u32 dev, u32* mfg, u32* type) {
    u32 status;
    s32 rv;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    IO_WRITE(PI_70_REG, 0);
    IO_WRITE(PI_48_REG, NAND_READ_ID(0, dev, TRUE));

    rv = __osBbCardWaitEvent();
    status = IO_READ(PI_NAND_DATA_BUFFER(0, 0));
    *mfg = NAND_ID_GET_MFG(status);
    *type = NAND_ID_GET_TYPE(status);
    __osBbCardMultiplane = NAND_ID_IS_MULTIPLANE(status);

    __osBbCardRelAccess();
    return rv;
}
