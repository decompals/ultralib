#include "macros.h"
#include "PR/os_internal.h"
#include "controller.h"
#include "siint.h"

static OSPifRam __MotorDataBuf[MAXCONTROLLERS];

s32 __osMotorAccess(OSPfs* pfs, u32 vibrate) {
    int i;
    s32 ret;
    u8* ptr = (u8*)&__MotorDataBuf[pfs->channel];

    if (!(pfs->status & 8)) {
        return 5;
    }

    __osSiGetAccess();
    __MotorDataBuf[pfs->channel].pifstatus = CONT_CMD_EXE;
    ptr += pfs->channel;
    for (i = 0; i < BLOCKSIZE; i++) {
        ((__OSContRamReadFormat*)ptr)->data[i] = vibrate;
    }

    __osContLastCmd = CONT_CMD_END;
    __osSiRawStartDma(OS_WRITE, &__MotorDataBuf[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);
    __osSiRawStartDma(OS_READ, &__MotorDataBuf[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);

    ret = ((__OSContRamReadFormat*)ptr)->rxsize & 0xC0;
    if (!ret) {
        if (!vibrate) {
            if (((__OSContRamReadFormat*)ptr)->datacrc != 0) {
                ret = PFS_ERR_CONTRFAIL;
            }
        } else {
            if (((__OSContRamReadFormat*)ptr)->datacrc != 0xEB) {
                ret = PFS_ERR_CONTRFAIL;
            }
        }
    }

    __osSiRelAccess();

    return ret;
}

static void _MakeMotorData(int channel, OSPifRam *mdata) {
    u8 *ptr = (u8 *)mdata->ramarray;
    __OSContRamReadFormat ramreadformat;
    int i;

    ramreadformat.dummy = CONT_CMD_NOP;
    ramreadformat.txsize = CONT_CMD_WRITE_MEMPACK_TX;
    ramreadformat.rxsize = CONT_CMD_WRITE_MEMPACK_RX;
    ramreadformat.cmd = CONT_CMD_WRITE_MEMPACK;
    ramreadformat.addrh = 0x600 >> 3;
    ramreadformat.addrl = (u8)(__osContAddressCrc(0x600) | (0x600 << 5));
    
    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *ptr++ = 0;
        }
    }

    *(__OSContRamReadFormat *)ptr = ramreadformat;
    ptr += sizeof(__OSContRamReadFormat);
    ptr[0] = CONT_CMD_END;
}

s32 osMotorInit(OSMesgQueue *mq, OSPfs *pfs, int channel)
{
    s32 ret;
    u8 temp[32];

    pfs->queue = mq;
    pfs->channel = channel;
    pfs->activebank = 0xFF;
    pfs->status = 0;

    ret = __osPfsSelectBank(pfs, 0xFE);
    
    if (ret == 2) {
        ret = __osPfsSelectBank(pfs, 0x80);
    }

    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(mq, channel, 0x400, temp);

    if (ret == 2) {
        ret = PFS_ERR_CONTRFAIL;
    }

    if (ret != 0) {
        return ret;
    }

    if (temp[31] == 254) {
        return PFS_ERR_DEVICE;
    }

    ret = __osPfsSelectBank(pfs, 0x80);
    if (ret == 2) {
        ret = PFS_ERR_CONTRFAIL;
    }
    
    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(mq, channel, 1024, temp);
    if (ret == 2) {
        ret = PFS_ERR_CONTRFAIL;
    }
    
    if (ret != 0) {
        return ret;
    }
    
    if (temp[31] != 0x80) {
        return PFS_ERR_DEVICE;
    }

    if (!(pfs->status & PFS_MOTOR_INITIALIZED)) {
        _MakeMotorData(channel, &__MotorDataBuf[channel]);
    }

    pfs->status = PFS_MOTOR_INITIALIZED;
    return 0;
}
