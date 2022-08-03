#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"
#include "voiceinternal.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osVoiceStopReadData(OSVoiceHandle* hd) {
#ifdef BBPLAYER
    return CONT_ERR_DEVICE;
#else
    s32 ret;
    s32 i;
    u8 stat;
    u8 temp[4];

    ERRCK(__osVoiceGetStatus(hd->__mq, hd->__channel, &stat));

    if (stat & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    if (hd->__mode == 0) {
        return CONT_ERR_INVALID;
    }

    *(u32*)temp = 0x700;
    ret = __osVoiceContWrite4(hd->__mq, hd->__channel, 0, temp);

    if (ret == 0) {
        i = 0;
        do {
            ret = __osVoiceCheckResult(hd, &stat);
            if (ret & 0xFF00) {
                if (((ret & 7) == 0) || ((ret & 7) == 7)) {
                    ret = 0;
                    hd->__mode = 0;
                } else {
                    ret = CONT_ERR_INVALID;
                }
            } else {
                hd->__mode = 0;
            }
            i++;
        } while ((ret == CONT_ERR_VOICE_NO_RESPONSE) && (i < 20));
    }

    return ret;
#endif
}