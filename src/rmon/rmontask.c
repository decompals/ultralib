#ifndef _FINALROM

#include "dbgproto.h"
#include "PR/os_internal.h"
#include "rmonint.h"
#include "PR/rcp.h"
#include "sptask.h"

// TODO: this comes from a header
#ident "$Revision: 1.4 $"

void __rmonMaskIdleThreadInts(void) {
    register OSThread* tptr = __osGetActiveQueue();

    while (tptr->priority != -1) {
        if (tptr->priority == OS_PRIORITY_IDLE) {
            tptr->context.sr &= ~OS_IM_CPU;
            tptr->context.sr |= (OS_IM_RDBREAD | OS_IM_RDBWRITE | OS_IM_CART);
            break;
        }
        tptr = tptr->tlnext;
    }
}

OSThread* __rmonGetTCB(int threadNumber) {
    register OSThread* tptr = __osGetActiveQueue();

    if (threadNumber < 1) {
        return NULL;
    }

    while (tptr->priority != -1) {
        if (tptr->id == threadNumber) {
            return tptr;
        }
        tptr = tptr->tlnext;
    }

    return NULL;
}

int __rmonStopUserThreads(int whichThread) {
    register int whichOne = 0;
    register OSThread* tptr = __osGetActiveQueue();

    ((void)"StopThreads %d\n");

    if (whichThread != 0) {
        // Stop specified thread

        while (tptr->priority != -1) {
            if (tptr->id == whichThread) {
                break;
            }
            tptr = tptr->tlnext;
        }

        if (tptr->priority == -1) {
            return 0;
        }

        if (tptr->priority > OS_PRIORITY_IDLE && tptr->priority <= OS_PRIORITY_APPMAX) {
            osStopThread(tptr);
            if (tptr->state != OS_STATE_STOPPED) {
                ((void)"Couldn't stop thread %d\n");
            }
            whichOne = whichThread;
        }
    } else {
        // Stop all threads

        while (tptr->priority != -1) {
            if (tptr->priority > OS_PRIORITY_IDLE && tptr->priority <= OS_PRIORITY_APPMAX) {
                osStopThread(tptr);
                if (tptr->state != OS_STATE_STOPPED) {
                    ((void)"Couldn\'t stop thread %d\n");
                }
                whichOne = -1;
            }
            tptr = tptr->tlnext;
        }
    }
    return whichOne;

}

int __rmonListThreads(KKHeader* req) {
    register KKObjectRequest* request = (KKObjectRequest*)req;
    KKObjsEvent* reply = (KKObjsEvent*)__rmonUtilityBuffer;

    ((void)"ListThreads\n");

    reply->object = (request->object == -1) ? RMON_PID_CPU : request->object;

    if (req->method == RMON_RSP) {
        reply->objs.number = 1;
        reply->objs.objects[0] = RMON_TID_RSP;
    } else {
        register OSThread* tptr = __osGetActiveQueue();
        reply->objs.number = 0;

        while (tptr->priority != -1) {
            if (tptr->id != 0) {
                reply->objs.objects[reply->objs.number] = tptr->id;
                reply->objs.number++;
            }
            tptr = tptr->tlnext;
        }
    }
    reply->header.code = request->header.code;
    reply->header.error = 0;
    __rmonSendReply(&reply->header, sizeof(*reply) + sizeof(reply->objs.objects[0]) * (reply->objs.number - 1),
                    1);
    return 0;

}

int __rmonGetThreadStatus(int method, int id, KKStatusEvent* reply) {
    u32 inst;

    ((void)"ThreadStatus %d method %d\n");

    reply->status.tid = id;
    reply->status.pid = (method == RMON_RSP) ? RMON_PID_RSP : RMON_PID_CPU;
    reply->status.why = 1;
    reply->status.what = 0;
    reply->status.info.major = 0;
    reply->status.info.minor = 0;
    reply->status.rv = 0;

    if (method == RMON_RSP) {
        reply->status.start = SP_IMEM_START;
        reply->status.priority = 42;
        if (__rmonRCPrunning()) {
            reply->status.flags = OS_STATE_RUNNING;
            reply->status.info.addr = 0;
            reply->status.instr = 0;
        } else {
            reply->status.flags = OS_STATE_STOPPED;
            reply->status.info.addr = __rmonReadWordAt((u32*)SP_PC_REG) + SP_IMEM_START;
            inst = __rmonReadWordAt((u32*)reply->status.info.addr);
            if ((inst & MIPS_BREAK_MASK) == MIPS_BREAK_OPCODE) {
                inst = 0xD;
            }
            if (__rmonRcpAtBreak) {
                reply->status.why = 2;
                reply->status.info.major = 2;
                reply->status.info.minor = 4;
            }
            reply->status.instr = inst;
        }
    } else {
        OSThread* tptr = __osGetActiveQueue();

        while (tptr->priority != -1) {
            if (tptr->id == id) {
                break;
            }
            tptr = tptr->tlnext;
        }
        if (tptr->priority == -1) {
            return -2;
        }

        reply->status.priority = tptr->priority;
        reply->status.flags = (tptr->state != 0) ? tptr->state : OS_STATE_STOPPED;
        reply->status.info.addr = tptr->context.pc;

        inst = *(u32*)(tptr->context.pc);
        if ((inst & MIPS_BREAK_MASK) == MIPS_BREAK_OPCODE) {
            inst = 0xD;
        }

        reply->status.instr = inst;
        reply->status.start = (int)tptr;

        if (tptr->flags & OS_FLAG_CPU_BREAK) {
            reply->status.why = 2;
            reply->status.info.major = 2;
            reply->status.info.minor = 4;
        } else if (tptr->flags & OS_FLAG_FAULT) {
            reply->status.why = 2;
            reply->status.info.major = 1;
            reply->status.info.minor = 2;
        }
    }

    return 0;
}

int __rmonThreadStatus(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKStatusEvent reply;

    if (__rmonGetThreadStatus(req->method, request->object, &reply) != 0) {
        return -2;
    }

    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

int __rmonStopThread(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKStatusEvent reply;
    u32* pc;

    ((void)"StopThread %d\n");

    switch (req->method) {
        case RMON_CPU:
            __rmonStopUserThreads(request->object);
            break;
        case RMON_RSP:
            if (__rmonRCPrunning()) {
                __rmonIdleRCP();
                pc = __rmonReadWordAt(SP_PC_REG);
                if (pc == NULL) {
                    break;
                }
                pc--;
                if ((u32)__rmonGetBranchTarget(TRUE, RMON_TID_RSP, (u32)pc + SP_IMEM_START) % 4 == 0) {
                    __rmonStepRCP();
                }
            }
            break;
        default:
            return -4;
    }

    if (__rmonGetThreadStatus(req->method, request->object, &reply) != 0) {
        return -2;
    }
    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    if (reply.status.flags == OS_STATE_STOPPED) {
        reply.header.code = 4;
        __rmonSendReply(&reply.header, sizeof(reply), 2);
    }
    return 0;

}

int __rmonRunThread(KKHeader* req) {
    KKRunThreadRequest* request = (KKRunThreadRequest*)req;
    KKObjectEvent reply;
    KKStatusEvent exceptionReply;
    register OSThread* tptr;
    register int runNeeded = FALSE;

    ((void)"RunThread %d\n");

    switch (req->method) {
        case RMON_CPU:
            tptr = __osGetActiveQueue();
            while (tptr->priority != -1) {
                if (tptr->id == request->tid) {
                    break;
                }
                tptr = tptr->tlnext;
            }

            if (tptr->priority == -1) {
                return -2;
            }
            if (tptr->state != OS_STATE_STOPPED) {
                return -4;
            }
            tptr->flags &= ~(OS_FLAG_CPU_BREAK | OS_FLAG_FAULT);
            if (request->actions.flags & RMON_RUNTHREAD_SETPC) {
                tptr->context.pc = request->actions.vaddr;
            }
            if ((request->actions.flags & RMON_RUNTHREAD_SSTEP) &&
                __rmonSetSingleStep(request->tid, (u32*)tptr->context.pc) == 0) {
                return -4;
            }
            runNeeded = TRUE;
            break;
        case RMON_RSP:
            if (__rmonRCPrunning()) {
                return -4;
            }
            if (request->actions.flags & RMON_RUNTHREAD_SETPC) {
                __rmonWriteWordTo(SP_PC_REG, request->actions.vaddr - SP_IMEM_START);
            }
            if (request->actions.flags & RMON_RUNTHREAD_SSTEP) {
                if ((u32)__rmonGetBranchTarget(TRUE, RMON_TID_RSP, __rmonReadWordAt(SP_PC_REG) + SP_IMEM_START) % 4 == 0) {
                    __rmonStepRCP();
                }
                __rmonStepRCP();
                __rmonRcpAtBreak = TRUE;
            } else {
                __rmonRcpAtBreak = FALSE;
                __rmonRunRCP();
            }
            reply.header.code = request->header.code;
            reply.header.error = 0;
            reply.object = request->tid;
            __rmonSendReply(&reply.header, sizeof(reply), 1);
            if (request->actions.flags & RMON_RUNTHREAD_SSTEP) {
                __rmonGetThreadStatus(RMON_RSP, RMON_TID_RSP, &exceptionReply);
                __rmonGetExceptionStatus(&exceptionReply);
                __rmonSendReply(&exceptionReply.header, sizeof(exceptionReply), 2);
            }
            return 0;
        default:
            return -4;
    }

    reply.header.code = request->header.code;
    reply.header.error = 0;
    reply.object = request->tid;
    __rmonSendReply(&reply.header, sizeof(reply), 1);

    if (runNeeded) {
        osStartThread(tptr);
    }
    return 1;
}

#endif
