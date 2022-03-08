#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"
#include "voiceinternal.h"

s32 __osVoiceSetADConverter(OSMesgQueue* mq, s32 channel, u8 data) {
    s32 ret;
    int i;
    u8* ptr;
    u8 status;
    // u16 sp48;
    int retry = 2;

    __osSiGetAccess();

    // sp48 = data * 8;

    do {

        ptr = (u8*)__osPfsPifRam.ramarray;

        if ((__osContLastCmd != 0xD) || (__osPfsLastChannel != channel)) {
            __osContLastCmd = 0xD;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++) {
                *ptr++ = 0;
            }

            __osPfsPifRam.pifstatus = CONT_CMD_READ_BUTTON;

            ptr[0] = 3;
            ptr[1] = 1;
            ptr[2] = 0xD;
            ptr[5] = 0;
            ptr[6] = 0xFE;
        } else {
            ptr = (u8*)&__osPfsPifRam + channel;
        }

        ptr[3] = data;
        ptr[4] = __osContAddressCrc(data * 8);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        ret = (ptr[1] & 0xC0) >> 4;

        if (ret == 0) {
            if (ptr[5] & 1) {
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
