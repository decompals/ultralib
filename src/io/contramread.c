#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "controller.h"
#include "siint.h"

s32 __osPfsLastChannel = -1;

s32 __osContRamRead(OSMesgQueue* mq, int channel, u16 address, u8* buffer) {
    s32 ret;
    s32 i;
    u8* ptr;
    s32 retry = 2;

    __osSiGetAccess();
    do {
        ptr = (u8*)&__osPfsPifRam;

        if ((__osContLastCmd != CONT_CMD_READ_MEMPACK) || (__osPfsLastChannel != channel)) {
            __osContLastCmd = CONT_CMD_READ_MEMPACK;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++) {
                *ptr++ = 0;
            }

            __osPfsPifRam.pifstatus = CONT_CMD_EXE;
            ((__OSContRamReadFormat*)ptr)->dummy = CONT_CMD_NOP;
            ((__OSContRamReadFormat*)ptr)->txsize = CONT_CMD_READ_MEMPACK_TX;
            ((__OSContRamReadFormat*)ptr)->rxsize = CONT_CMD_READ_MEMPACK_RX;
            ((__OSContRamReadFormat*)ptr)->cmd = CONT_CMD_READ_MEMPACK;
            ((__OSContRamReadFormat*)ptr)->datacrc = 0xFF;
            ptr[sizeof(__OSContRamReadFormat)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }

        ((__OSContRamReadFormat*)ptr)->addrh = address >> 3;
        ((__OSContRamReadFormat*)ptr)->addrl = (s8)(__osContAddressCrc(address) | (address << 5));
        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        ret = (((__OSContRamReadFormat*)ptr)->rxsize & 0xC0) >> 4;
        if (!ret) {
            if (__osContDataCrc(ptr + 6) != ((__OSContRamReadFormat*)ptr)->datacrc) {
                ret = __osPfsGetStatus(mq, channel);
                if (ret) {
                    break;
                }
                ret = PFS_ERR_CONTRFAIL;
            } else {
                bcopy(ptr + 6, buffer, BLOCKSIZE);
            }
        } else {
            ret = PFS_ERR_NOPACK;
        }
        if (ret != PFS_ERR_CONTRFAIL) {
            break;
        }
    } while (0 <= retry--);
    __osSiRelAccess();

    return ret;
}