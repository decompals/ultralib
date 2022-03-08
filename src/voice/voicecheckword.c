#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"

s32 osVoiceCheckWord(u8* word) {
    s32 k;
    s32 ret = 0;
    u16 sjis;
    u16 old = 0;

    for (k = 0; word[k] != 0; k += 2) {
        sjis = (word[k] << 8) + word[k + 1];
        if (((sjis != 0x815B) && (sjis < 0x829F)) ||
            ((sjis >= 0x82F2) && (sjis < 0x8340)) || (sjis == 0x837F) ||
            (sjis >= 0x8397)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if ((k == 0) && ((sjis == 0x815B) || (sjis == 0x82F1) || (sjis == 0x82C1) ||
                                (sjis == 0x82EC) || (sjis == 0x8393) || (sjis == 0x8362) ||
                                (sjis == 0x838E) || (sjis == 0x8395) || (sjis == 0x8396))) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x829F) || (sjis == 0x8340)) && (old != 0x82D3) &&
                   (old != 0x8374) && (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82A1) || (sjis == 0x8342)) && (old != 0x82A4) &&
                   (old != 0x82C4) && (old != 0x82C5) && (old != 0x82D3) &&
                   (old != 0x8345) && (old != 0x8365) && (old != 0x8366) &&
                   (old != 0x8374) && (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82A3) || (sjis == 0x8344)) && (old != 0x82C6) &&
                   (old != 0x82C7) && (old != 0x82D3) && (old != 0x8367) &&
                   (old != 0x8368) && (old != 0x8374) && (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82A5) || (sjis == 0x8346)) && (old != 0x82A4) &&
                   (old != 0x82B5) && (old != 0x82B6) && (old != 0x82BF) &&
                   (old != 0x82C0) && (old != 0x82C2) && (old != 0x82D3) &&
                   (old != 0x8345) && (old != 0x8356) && (old != 0x8357) &&
                   (old != 0x8360) && (old != 0x8361) && (old != 0x8363) &&
                   (old != 0x8374) && (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82A7) || (sjis == 0x8348)) && (old != 0x82A4) &&
                   (old != 0x82D3) && (old != 0x8345) && (old != 0x8374) &&
                   (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82E1) || (sjis == 0x82E3) || (sjis == 0x82E5) ||
                    (sjis == 0x8383) || (sjis == 0x8385) || (sjis == 0x8387)) &&
                   (old != 0x82AB) && (old != 0x82B5) && (old != 0x82BF) &&
                   (old != 0x82C9) && (old != 0x82D0) && (old != 0x82DD) &&
                   (old != 0x82E8) && (old != 0x82AC) && (old != 0x82B6) &&
                   (old != 0x82C0) && (old != 0x82D1) && (old != 0x82D2) &&
                   (old != 0x834C) && (old != 0x8356) && (old != 0x8360) &&
                   (old != 0x836A) && (old != 0x8371) && (old != 0x837E) &&
                   (old != 0x838A) && (old != 0x834D) && (old != 0x8357) &&
                   (old != 0x8361) && (old != 0x8372) && (old != 0x8373) &&
                   (old != 0x8394)) {
            ret = CONT_ERR_VOICE_WORD;
        } else if ((sjis == 0x815B) && ((old == 0x82F1) || (old == 0x82C1) ||
                                                (old == 0x8393) || (old == 0x8362))) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((sjis == 0x82F1) || (sjis == 0x8393)) &&
                   ((old == 0x82F1) || (old == 0x8393))) {
            ret = CONT_ERR_VOICE_WORD;
        } else if (((old == 0x82C1) || (old == 0x8362)) &&
                   ((sjis == 0x815B) || (sjis == 0x82A0) || (sjis == 0x82A2) ||
                    (sjis == 0x82A4) || (sjis == 0x82A6) || (sjis == 0x82A8) ||
                    (sjis == 0x82C8) || (sjis == 0x82C9) || (sjis == 0x82CA) ||
                    (sjis == 0x82CB) || (sjis == 0x82CC) || (sjis == 0x82DC) ||
                    (sjis == 0x82DD) || (sjis == 0x82DE) || (sjis == 0x82DF) ||
                    (sjis == 0x82E0) || (sjis == 0x82E2) || (sjis == 0x82E4) ||
                    (sjis == 0x82E6) || (sjis == 0x82E7) || (sjis == 0x82E8) ||
                    (sjis == 0x82E9) || (sjis == 0x82EA) || (sjis == 0x82EB) ||
                    (sjis == 0x82ED) || (sjis == 0x82EE) || (sjis == 0x82EF) ||
                    (sjis == 0x82F0) || (sjis == 0x82F1) || (sjis == 0x829F) ||
                    (sjis == 0x82A1) || (sjis == 0x82A3) || (sjis == 0x82A5) ||
                    (sjis == 0x82A7) || (sjis == 0x82E1) || (sjis == 0x82E3) ||
                    (sjis == 0x82E5) || (sjis == 0x82C1) || (sjis == 0x82EC) ||
                    (sjis == 0x8341) || (sjis == 0x8343) || (sjis == 0x8345) ||
                    (sjis == 0x8347) || (sjis == 0x8349) || (sjis == 0x8369) ||
                    (sjis == 0x836A) || (sjis == 0x836B) || (sjis == 0x836C) ||
                    (sjis == 0x836D) || (sjis == 0x837D) || (sjis == 0x837E) ||
                    (sjis == 0x8380) || (sjis == 0x8381) || (sjis == 0x8382) ||
                    (sjis == 0x8384) || (sjis == 0x8386) || (sjis == 0x8388) ||
                    (sjis == 0x8389) || (sjis == 0x838A) || (sjis == 0x838B) ||
                    (sjis == 0x838C) || (sjis == 0x838D) || (sjis == 0x838F) ||
                    (sjis == 0x8390) || (sjis == 0x8391) || (sjis == 0x8392) ||
                    (sjis == 0x8393) || (sjis == 0x8340) || (sjis == 0x8342) ||
                    (sjis == 0x8344) || (sjis == 0x8346) || (sjis == 0x8348) ||
                    (sjis == 0x8383) || (sjis == 0x8385) || (sjis == 0x8387) ||
                    (sjis == 0x8362) || (sjis == 0x838E))) {
            ret = CONT_ERR_VOICE_WORD;
        }

        if (ret != 0) {
            return ret;
        }

        old = sjis;
    }

    if ((sjis == 0x82C1) || (sjis == 0x8362)) {
        ret = CONT_ERR_VOICE_WORD;
    }

    return ret;
}