#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"
#include "voiceinternal.h"

s32 __osVoiceContWrite20(OSMesgQueue* mq, int channel, u16 address, u8* buffer) {
    s32 ret;
    u8 status;
    int i;
    u8* ptr;
    int retry;
    u8 crc;

    retry = 2;

    __osSiGetAccess();

    do {

        ptr = (u8*)&__osPfsPifRam;

        if ((__osContLastCmd != 0xA) || (__osPfsLastChannel != channel)) {
            __osContLastCmd = 0xA;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++) {
                *ptr++ = 0;
            }

            __osPfsPifRam.pifstatus = CONT_CMD_READ_BUTTON;

            ptr[0] = 0xFF;
            ptr[1] = 0x17;
            ptr[2] = 1;
            ptr[3] = 0xA;
            ptr[0x1A] = 0xFF;
            ptr[0x1B] = 0xFE;
        } else {
            ptr = (u8*)&__osPfsPifRam + channel;
        }

        ptr[4] = address >> 3;
        ptr[5] = __osContAddressCrc(address) | (address << 5);

        bcopy(buffer, &ptr[6], 20);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        crc = __osVoiceContDataCrc(buffer, 20);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        ret = (ptr[2] & 0xC0) >> 4;

        if (ret == 0) {
            if (crc != ptr[0x1A]) {
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

