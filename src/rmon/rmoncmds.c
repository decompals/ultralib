#ifndef _FINALROM

#include "dbgproto.h"
#include "rmonint.h"

// TODO: this comes from a header
#ident "$Revision: 1.4 $"

static int NotImplemented(KKHeader* dummy) {
    return -1;
}

static FUNPTR dispatchTable[] = {
    __rmonLoadProgram,      __rmonListProcesses,    __rmonGetExeName,       __rmonListThreads,
    __rmonThreadStatus,     NotImplemented,         __rmonStopThread,       __rmonRunThread,
    NotImplemented,         NotImplemented,         __rmonSetFault,         NotImplemented,
    __rmonGetRegionCount,   __rmonGetRegions,       __rmonGetGRegisters,    __rmonSetGRegisters,
    __rmonGetFRegisters,    __rmonSetFRegisters,    __rmonReadMem,          __rmonWriteMem,
    __rmonSetBreak,         __rmonClearBreak,       __rmonListBreak,        NotImplemented,
    NotImplemented,         NotImplemented,         NotImplemented,         NotImplemented,
    NotImplemented,         NotImplemented,         __rmonSetComm,          NotImplemented,
    NotImplemented,         NotImplemented,         NotImplemented,         NotImplemented,
    NotImplemented,         NotImplemented,         NotImplemented,         NotImplemented,
    NotImplemented,         NotImplemented,         NotImplemented,         NotImplemented,
    NotImplemented,         NotImplemented,         NotImplemented,         NotImplemented,
    NotImplemented,         __rmonGetSRegs,         __rmonSetSRegs,         __rmonGetVRegs,
    __rmonSetVRegs,         NotImplemented,
};

int __rmonExecute(KKHeader* request) {
    int retval;
    KKHeader reply;

    if (request->code >= (int)(sizeof(dispatchTable) / sizeof(dispatchTable[0])) - 1) {
        return -1;
    }

    retval = dispatchTable[request->code](request);
    if (retval < 0) {
        reply.code = request->code;
        reply.error = retval;
        __rmonSendReply(&reply, sizeof(KKHeader), 1);
    }
    return retval;
}

#endif
