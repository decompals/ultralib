#ifndef _FINALROM

#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os.h"
#include "G/os_internal.h"
#define _ULTRALIB_G_RMONBRK
#define _ULTRALIB_G_RMONMISC
#endif
#include "PRinternal/dbgproto.h"
#if !(defined(_DEBUG) && BUILD_VERSION <= VERSION_G)
#include "PR/os_internal.h"
#endif
#include "PR/sptask.h"
#include "PRinternal/rmonint.h"
#if defined(_ULTRALIB_G_RMONBRK)
#undef _ULTRALIB_G_RMONMISC
#undef _ULTRALIB_G_RMONBRK
#endif

#include "PRinternal/macros.h"

// TODO: this comes from a header
#if BUILD_VERSION >= VERSION_J
#ident "$Revision: 1.4 $"
#else
#ident "$Revision: 3.70 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.2 $"
#ident "$Revision: 1.4 $"
#ident "$Revision: 1.3 $"
#endif

#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
static OSThread rmonIOThread ALIGNED(0x8);
static OSMesg rmonMsgs[8] ALIGNED(0x8);
static STACK(rmonIOStack, 0x4000) ALIGNED(0x10);
static OSMesg rmonPiMsgs[8] ALIGNED(0x8);
static OSMesgQueue rmonPiMQ ALIGNED(0x8);
OSMesgQueue __rmonMQ ALIGNED(0x8);
extern OSMesgQueue rmonCalloutMQ;
extern OSMesgQueue __rmonLogAckMesgQueue;
extern OSMesgQueue __rmonDataAckMesgQueue;
extern OSMesgQueue __rmonProfReqMesgQueue;
extern OSMesgQueue __rmonProfAckMesgQueue;
#endif

int __rmonSetFault(KKHeader* req) {
    KKFaultRequest* request = (KKFaultRequest*)req;
    KKObjectEvent reply;

    STUBBED_PRINTF(("SetFault\n"));

    reply.header.code = request->header.code;
    reply.header.error = TV_ERROR_NO_ERROR;
    reply.object = request->tid;

    __rmonSendReply(&reply.header, sizeof(reply), KK_TYPE_REPLY);
    return TV_ERROR_NO_ERROR;
}

#if !(defined(_DEBUG) && BUILD_VERSION <= VERSION_G)
OSMesgQueue __rmonMQ ALIGNED(0x8);
static OSThread rmonIOThread ALIGNED(0x8);
static OSMesg rmonMsgs[8] ALIGNED(0x8);
static STACK(rmonIOStack, 0x4000) ALIGNED(0x10);
static OSMesg rmonPiMsgs[8] ALIGNED(0x8);
static OSMesgQueue rmonPiMQ ALIGNED(0x8);
#endif

void __rmonInit(void) {
    osCreateMesgQueue(&__rmonMQ, rmonMsgs, ARRLEN(rmonMsgs));
    osSetEventMesg(OS_EVENT_CPU_BREAK, &__rmonMQ, (OSMesg)RMON_MESG_CPU_BREAK);
    osSetEventMesg(OS_EVENT_SP_BREAK, &__rmonMQ, (OSMesg)RMON_MESG_SP_BREAK);
    osSetEventMesg(OS_EVENT_FAULT, &__rmonMQ, (OSMesg)RMON_MESG_FAULT);
    osSetEventMesg(OS_EVENT_THREADSTATUS, &__rmonMQ, NULL);
    osCreateThread(&rmonIOThread, 0, (void (*)(void*))__rmonIOhandler, NULL, STACK_START(rmonIOStack),
                   OS_PRIORITY_MAX);
    osCreatePiManager(OS_PRIORITY_PIMGR, &rmonPiMQ, rmonPiMsgs, ARRLEN(rmonPiMsgs));
    osStartThread(&rmonIOThread);
}

void __rmonPanic(void) {
    STUBBED_PRINTF(("PANIC!!\n"));

    for (;;) {
        ;
    }
}

int __rmonSetComm(KKHeader* req) {
    KKObjectEvent reply;

    STUBBED_PRINTF(("SetComm\n"));

    reply.header.code = req->code;
    reply.object = 0;
    reply.header.error = TV_ERROR_NO_ERROR;

    __rmonSendReply(&reply.header, sizeof(reply), KK_TYPE_REPLY);

    return TV_ERROR_NO_ERROR;
}

#endif
