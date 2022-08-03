#include "PR/os_internal.h"
#include "controller.h"
#include "siint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void __osPackResetData(void);

s32 osContReset(OSMesgQueue *mq, OSContStatus *data) {
    u8 pattern;
    s32 ret;
    
    __osSiGetAccess();

    if (__osContLastCmd != CONT_CMD_RESET) {
#ifdef BBPLAYER
        __osPackRequestData(CONT_CMD_RESET);
#else
        __osPackResetData();
#endif

        ret = __osSiRawStartDma(OS_WRITE, __osContPifRam.ramarray);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

#ifndef BBPLAYER
        ret = __osSiRawStartDma(OS_READ, __osContPifRam.ramarray);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        
        __osPackRequestData(CONT_CMD_RESET);

        ret = __osSiRawStartDma(OS_WRITE, __osContPifRam.ramarray);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        __osContLastCmd = CONT_CMD_RESET;
#else
        __osContLastCmd = CONT_CMD_CHANNEL_RESET;
#endif

    }

    ret = __osSiRawStartDma(OS_READ, __osContPifRam.ramarray);
    osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    
    __osContGetInitData(&pattern, data);
    __osSiRelAccess();
    
    return ret;
}

void __osPackResetData(void) {
    u8 *ptr;
    int i;

    __osContPifRam.pifstatus = CONT_CMD_EXE;
    ptr = __osContPifRam.ramarray;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        *ptr++ = CONT_CMD_CHANNEL_RESET;
    }
    
    *ptr = CONT_CMD_END;
}
