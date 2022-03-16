#ifndef _FINALROM

#include "dbgproto.h"
#include "PR/os_internal.h"
#include "PR/sptask.h"
#include "rmonint.h"

#include "macros.h"

// TODO: this comes from a header
#ident "$Revision: 1.4 $"

OSMesgQueue __rmonMQ ALIGNED(8);

int __rmonSetFault(KKHeader* req) {
    KKFaultRequest* request = (KKFaultRequest*)req;
    KKObjectEvent reply;

    ((void)"SetFault\n");

    reply.header.code = request->header.code;
    reply.header.error = 0;
    reply.object = request->tid;

    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;

}

static OSThread rmonIOThread ALIGNED(8);
static OSMesg rmonMsgs[8] ALIGNED(8);
static u64 rmonIOStack[2048] ALIGNED(8);
static OSMesg rmonPiMsgs[8] ALIGNED(8);
static OSMesgQueue rmonPiMQ ALIGNED(8);

void __rmonInit(void) {
    osCreateMesgQueue(&__rmonMQ, rmonMsgs, sizeof(rmonMsgs)/sizeof(rmonMsgs[0]));
    osSetEventMesg(OS_EVENT_CPU_BREAK, &__rmonMQ, RMON_MESG_CPU_BREAK);
    osSetEventMesg(OS_EVENT_SP_BREAK, &__rmonMQ, RMON_MESG_SP_BREAK);
    osSetEventMesg(OS_EVENT_FAULT, &__rmonMQ, RMON_MESG_FAULT);
    osSetEventMesg(OS_EVENT_THREADSTATUS, &__rmonMQ, NULL);
    osCreateThread(&rmonIOThread, 0, (void (*)(void*))__rmonIOhandler, NULL,
                   rmonIOStack + sizeof(rmonIOStack)/sizeof(rmonIOStack[0]), OS_PRIORITY_MAX);
    osCreatePiManager(OS_PRIORITY_PIMGR, &rmonPiMQ, rmonPiMsgs, sizeof(rmonPiMsgs)/sizeof(rmonPiMsgs[0]));
    osStartThread(&rmonIOThread);
}

void __rmonPanic(void) {
    ((void)"PANIC!!\n");

    for (;;) {
        ;
    }
}

int __rmonSetComm(KKHeader* req) {
    KKObjectEvent reply;

    ((void)"SetComm\n");

    reply.header.code = req->code;
    reply.object = 0;
    reply.header.error = 0;

    __rmonSendReply(&reply.header, sizeof(reply), 1);

    return 0;
}

#endif
