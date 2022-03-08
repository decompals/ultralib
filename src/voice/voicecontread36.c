#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"
#include "voiceinternal.h"

s32 __osVoiceContRead36(OSMesgQueue* mq, s32 channel, u16 address, u8* buffer) {
    s32 ret;
    u8 status;
    s32 i;
    u8* ptr;
    s32 retry = 2;

    __osSiGetAccess();

    do {

        ptr = (u8*)&__osPfsPifRam;

        if ((__osContLastCmd != 9) || (__osPfsLastChannel != channel)) {
            __osContLastCmd = 9;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++) {
                *ptr++ = 0;
            }

            __osPfsPifRam.pifstatus = CONT_CMD_READ_BUTTON;

            ptr[0] = 0xFF;
            ptr[1] = 3;
            ptr[2] = 0x25;
            ptr[3] = 9;
            ptr[0x2A] = 0xFF;
            ptr[0x2B] = 0xFE;
        } else {
            ptr = (u8*)&__osPfsPifRam + channel;
        }

        ptr[4] = address >> 3;
        ptr[5] = __osContAddressCrc(address) | (address << 5);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        ret = (ptr[2] & 0xC0) >> 4;

        if (ret == 0) {
            if (__osVoiceContDataCrc(&ptr[6], 36) != ptr[42]) {
                ret = __osVoiceGetStatus(mq, channel, &status);
                if (ret != 0) {
                    break;
                }

                ret = CONT_ERR_CONTRFAIL;
            } else {
                bcopy(&ptr[6], buffer, 36);
            }
        } else {
            ret = CONT_ERR_NO_CONTROLLER;
        }

    } while ((ret == CONT_ERR_CONTRFAIL) && (retry-- >= 0));

    __osSiRelAccess();

    return ret;
}
