#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "macros.h"

#ident "$Revision: 1.1 $"

extern u8 __osBbCardChange;

static OSMesgQueue cardEventQueue;
static OSMesg cardEventBuf[5];
static OSIoMesg cardFlashMsg;
static OSIoMesg cardMdMsg;

#define FLASH_MSG 17
#define MD_MSG    18

void __osBbCardInitEvent(void) {
    osCreateMesgQueue(&cardEventQueue, cardEventBuf, ARRLEN(cardEventBuf));
    cardFlashMsg.hdr.type = FLASH_MSG;
    cardFlashMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    cardFlashMsg.hdr.retQueue = NULL;
    cardMdMsg.hdr.type = MD_MSG;
    cardMdMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    cardMdMsg.hdr.retQueue = NULL;
    osSetEventMesg(OS_EVENT_FLASH, &cardEventQueue, &cardFlashMsg);
    osSetEventMesg(OS_EVENT_MD, &cardEventQueue, &cardMdMsg);
}

s32 __osBbCardFlushEvent(void) {
    s32 rv = FALSE;
    OSIoMesg* mb;

    while (osRecvMesg(&cardEventQueue, (OSMesg*)&mb, OS_MESG_NOBLOCK) != -1) {
        rv = TRUE;
#ifdef _DEBUG
        osSyncPrintf("flush cardEventQueue: %d\n", mb->hdr.type);
#endif
    }
    return rv;
}

s32 __osBbCardWaitEvent(void) {
    OSIoMesg* mb;

    osRecvMesg(&cardEventQueue, (OSMesg*)&mb, OS_MESG_BLOCK);

    switch (mb->hdr.type) {
        case FLASH_MSG:
            return 0;
        case MD_MSG:
#ifdef _DEBUG
            osSyncPrintf("md interrupt\n");
#endif
            __osBbCardChange = TRUE;
            return BBCARD_ERR_CHANGED;
        default:
            return BBCARD_ERR_FAIL;
    }
}
