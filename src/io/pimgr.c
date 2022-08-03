#include "macros.h"
#include "PR/os_internal.h"
#include "piint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

OSDevMgr __osPiDevMgr = {0};
OSPiHandle *__osPiTable = NULL;
OSPiHandle __Dom1SpeedParam ALIGNED(8);
OSPiHandle __Dom2SpeedParam ALIGNED(8);
OSPiHandle *__osCurrentHandle[2] ALIGNED(8) = {&__Dom1SpeedParam, &__Dom2SpeedParam};
static OSThread piThread;
static char piThreadStack[OS_PIM_STACKSIZE];
static OSMesgQueue piEventQueue;
static OSMesg piEventBuf[1];

void osCreatePiManager(OSPri pri, OSMesgQueue *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt) {
    u32 savedMask;
    OSPri oldPri;
    OSPri myPri;

    if (!__osPiDevMgr.active) {
        osCreateMesgQueue(cmdQ, cmdBuf, cmdMsgCnt);
        osCreateMesgQueue(&piEventQueue, (OSMesg*)piEventBuf, 1);
        
        if (!__osPiAccessQueueEnabled) {
            __osPiCreateAccessQueue();
        }
        
        osSetEventMesg(OS_EVENT_PI, &piEventQueue, (OSMesg)0x22222222);
        oldPri = -1;
        myPri = osGetThreadPri(NULL);

        if (myPri < pri) {
            oldPri = myPri;
            osSetThreadPri(NULL, pri);
        }

        savedMask = __osDisableInt();
        __osPiDevMgr.active = 1;
        __osPiDevMgr.thread = &piThread;
        __osPiDevMgr.cmdQueue = cmdQ;
        __osPiDevMgr.evtQueue = &piEventQueue;
        __osPiDevMgr.acsQueue = &__osPiAccessQueue;
        __osPiDevMgr.dma = __osPiRawStartDma;
        __osPiDevMgr.edma = __osEPiRawStartDma;
#ifdef BBPLAYER
        osCreateThread(&piThread, 0xCE5, __osDevMgrMain, &__osPiDevMgr, &piThreadStack[OS_PIM_STACKSIZE], pri);
#else
        osCreateThread(&piThread, 0, __osDevMgrMain, &__osPiDevMgr, &piThreadStack[OS_PIM_STACKSIZE], pri);
#endif
        osStartThread(&piThread);
        __osRestoreInt(savedMask);

        if (oldPri != -1) {
            osSetThreadPri(NULL, oldPri);
        }
    }
}
