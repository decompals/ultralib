#include "macros.h"
#include "PR/os_internal.h"
#include "controller.h"
#include "controller_gbpak.h"

OSTimer __osGbpakTimer;
OSMesg __osGbpakTimerMsg;
OSMesgQueue __osGbpakTimerQ ALIGNED(8);

s32 osGbpakInit(OSMesgQueue* mq, OSPfs* pfs, int channel) {
    int i;
    s32 ret;
    u8 temp[BLOCKSIZE];

    pfs->status = 0;
    
    for (i = 0; i < BLOCKSIZE; i++) {
        temp[i] = 0xFE;
    }

    ret = __osContRamWrite(mq, channel, 0x400U, temp, FALSE);
    if (ret == PFS_ERR_NEW_PACK) {
        ret = __osContRamWrite(mq, channel, 0x400U, temp, FALSE);
    }

    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(mq, channel, 0x400U, temp);

    if (ret == PFS_ERR_NEW_PACK) {
        ret = PFS_ERR_CONTRFAIL;
    }

    if (ret != 0) {
        return ret;
    } else {
        if (temp[BLOCKSIZE - 1] == 0xFE) {
            return PFS_ERR_DEVICE;
        }
    }

    for (i = 0; i < BLOCKSIZE; i++) {
        temp[i] = 0x84;
    }

    ret = __osContRamWrite(mq, channel, 0x400U, temp, FALSE);

    if (ret == PFS_ERR_NEW_PACK) {
        ret = PFS_ERR_CONTRFAIL;
    }

    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(mq, channel, 0x400U, temp);

    if (ret == PFS_ERR_NEW_PACK) {
        ret = PFS_ERR_CONTRFAIL;
    }

    if (ret != 0) {
        return ret;
    } else {
        if (temp[BLOCKSIZE - 1] != 0x84) {
            return PFS_ERR_DEVICE;
        }
    }

    ERRCK(__osPfsGetStatus(mq, channel));

    osCreateMesgQueue(&__osGbpakTimerQ, &__osGbpakTimerMsg, 1);
    osSetTimer(&__osGbpakTimer, 9000000, 0, &__osGbpakTimerQ, &__osGbpakTimerMsg);
    osRecvMesg(&__osGbpakTimerQ, NULL, 1);
    pfs->queue = mq;
    pfs->status = PFS_GBPAK_INITIALIZED;
    pfs->channel = channel;
    pfs->activebank = 0x84;
    pfs->banks = 0xFF;
    pfs->version = 0xFF;
    pfs->dir_size = 0xFF;

    return 0;
}
