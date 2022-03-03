#include "PR/os_internal.h"
#include "../io/controller.h"
#include "PR/os_voice.h"

s32 osVoiceInit(OSMesgQueue* mq, OSVoiceHandle* handle, int channel) {
    s32 ret;
    s32 i;
    u8 stat = 0;
    u8 buf[4];
    static u8 cmd[] = {0x1E, 0x6E, 0x08, 0x56, 0x03};

    handle->__channel = channel;
    handle->__mq = mq;
    handle->__mode = 0;

    ret = __osVoiceGetStatus(mq, channel, &stat);
    if (ret != 0) {
        return ret;
    }

    if (__osContChannelReset(mq, channel) != 0) {
        return CONT_ERR_CONTRFAIL;
    }

    for (i = 0; i < 5; i++) {
        ret = __osVoiceSetADConverter(mq, channel, cmd[i]);

        if (ret != 0) {
            return ret;
        }
    }

    ret = __osVoiceGetStatus(mq, channel, &stat);
    if (ret != 0) {
        return ret;
    }
    if (stat & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    *(u32*)buf = 0x100;
    ret = __osVoiceContWrite4(mq, channel, 0, buf);
    if (ret != 0) {
        return ret;
    }

    ret = __osVoiceCheckResult(handle, &stat);
    if (ret & 0xFF00) {
        ret = CONT_ERR_INVALID;
    }

    return ret;
}