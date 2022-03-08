#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"
#include "voiceinternal.h"

s32 __osVoiceContWrite4(OSMesgQueue* mq, int channel, u16 address, u8 dst[4]) {
    s32 ret;
    u8 status;
    int i;
    u8* ptr;
    s32 retry = 2;
    u8 crc;

    __osSiGetAccess();

    do {

        ptr = (u8*)&__osPfsPifRam;

        if ((__osContLastCmd != 0xC) || (__osPfsLastChannel != channel)) {
            __osContLastCmd = 0xC;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++) {
                *ptr++ = 0;
            }

            __osPfsPifRam.pifstatus = CONT_CMD_READ_BUTTON;

            ptr[0] = 0xFF;
            ptr[1] = 7;
            ptr[2] = 1;
            ptr[3] = 0xC;
            ptr[0xA] = 0xFF;
            ptr[0xB] = 0xFE;
        } else {
            ptr = (u8*)&__osPfsPifRam + channel;
        }

        ptr[4] = address >> 3;
        ptr[5] = __osContAddressCrc(address) | (address << 5);

        bcopy(dst, &ptr[6], 4);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        crc = __osVoiceContDataCrc(dst, 4);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        ret = (ptr[2] & 0xC0) >> 4;

        if (ret == 0) {
            if (crc != ptr[0xA]) {
                ret = __osVoiceGetStatus(mq, channel, &status);
                if (ret != 0) {
                    break;
                }

                ret = CONT_ERR_CONTRFAIL;
            }
        } else {
            ret = CONT_ERR_NO_CONTROLLER;
        }

    } while ((ret == CONT_ERR_CONTRFAIL) && (retry-- >= 0));

    __osSiRelAccess();

    return ret;
}