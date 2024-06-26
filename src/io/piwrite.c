#include "PRinternal/piint.h"
#include "PR/ultraerror.h"

s32 osPiWriteIo(u32 devAddr, u32 data) {
    register s32 ret;

#ifdef _DEBUG
    if (devAddr & 0x3) {
        __osError(ERR_OSPIWRITEIO, 1, devAddr);
        return -1;
    }
#endif

    __osPiGetAccess();
    ret = __osPiRawWriteIo(devAddr, data);
    __osPiRelAccess();

    return ret;
}
