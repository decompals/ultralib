#include "PR/os_internal.h"
#include "controller.h"

s32 __osPfsSelectBank(OSPfs* pfs, u8 bank) {
    u8 temp[BLOCKSIZE];
    int i;
    s32 ret = 0;

    for (i = 0; i < BLOCKSIZE; i++) {
        temp[i] = bank;
    }

    ret = __osContRamWrite(pfs->queue, pfs->channel, 0x8000 / BLOCKSIZE, temp, 0);

    if (ret == 0) {
        pfs->activebank = bank;
    }

    return ret;
}