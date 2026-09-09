#include "PR/os_internal.h"
#include "PRinternal/controller.h"
#include "PRinternal/siint.h"

s32 osPfsInit(OSMesgQueue* queue, OSPfs* pfs, int channel) {
    s32 ret = 0;

    __osSiGetAccess();
    ret = __osPfsGetStatus(queue, channel);
    __osSiRelAccess();

    if (ret != 0) {
        return ret;
    }

    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;
#if BUILD_VERSION >= VERSION_I
    pfs->activebank = -1;
#endif
    ERRCK(__osGetId(pfs));

    ret = osPfsChecker(pfs);
    pfs->status |= PFS_INITIALIZED;
    return ret;
}

#if BUILD_VERSION < VERSION_I
s32 __osPfsGetStatus(OSMesgQueue* queue, int channel) {
    s32 ret = 0;
    OSMesg dummy;
    u8 pattern;
    OSContStatus data[MAXCONTROLLERS];

    __osPfsRequestData(CONT_CMD_REQUEST_STATUS);
    ret = __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, OS_MESG_BLOCK);
    ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, OS_MESG_BLOCK);
    __osPfsGetInitData(&pattern, &data[0]);

    if (((data[channel].status & CONT_CARD_ON) != 0) && ((data[channel].status & CONT_CARD_PULL) != 0)) {
        return PFS_ERR_NEW_PACK;
    } else if ((data[channel].errno != 0) || ((data[channel].status & CONT_CARD_ON) == 0)) {
        return PFS_ERR_NOPACK;
    } else if ((data[channel].status & CONT_ADDR_CRC_ER) != 0) {
        return PFS_ERR_CONTRFAIL;
    }
    return ret;
}
#endif
