#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"

s32 osVoiceSetWord(OSVoiceHandle *hd, u8 *word) {
    s32 j;
    s32 k;
    s32 ret;
    u8 stat;
    u8 buf[40];

    ERRCK(__osVoiceGetStatus(hd->__mq, hd->__channel, &stat));
    if (stat & 2) {
        return 0xF;
    }
    for (k = 0; word[k] != 0; k += 2) {
        ;
    }
    // if (k >= 0x22) {
    //     return 0xE;
    // }
    bzero(buf, 40);

    for (j = 0; j < k; j += 2) {
        buf[39 - k + j] = word[j];
        buf[39 - k + j - 1] = word[j + 1];
    }

    buf[39 - j - 5] = 3;

    if (k >= 15) {
        ERRCK(__osVoiceContWrite20(hd->__mq, hd->__channel, 0, buf));
    }
    ERRCK(__osVoiceContWrite20(hd->__mq, hd->__channel, 0, buf + 20));
    ret = __osVoiceCheckResult(hd, &stat);
    if (ret != 0) {
        if (ret & 0x100) {
            ret = 0xD;
        } else if (ret & 0x200) {
            ret = 0xE;
        } else if (ret & 0xFF00) {
            ret = 5;
        }
    }
    return ret;
}
