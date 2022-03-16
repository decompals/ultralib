#ifndef _FINALROM

#include "dbgproto.h"
#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "PR/sptask.h"
#include "rmonint.h"
#include "PR/rdb.h"

// TODO: these come from headers
#ident "$Revision: 1.4 $"
#ident "$Revision: 3.70 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.2 $"
#ident "$Revision: 1.4 $"

static u32 RCPpc;
static u32 oldIMEMvalue;
static u32 DMEMbuffer[4];

typedef union {
    u32 everything;
    struct {
        int opcode : 6;
        int base : 5;
        int rt : 5;
        int offset : 16;
    } scalarop;
    struct {
        int opcode : 6;
        int base : 5;
        int rt : 5;
        int size : 5;
        int element : 4;
        int offset : 7;
    } vectorop;
} INSTRUCTION;

static void LoadStoreSU(int opcode, int regno) {
    INSTRUCTION inst;

    inst.everything = 0;
    inst.scalarop.opcode = opcode;
    inst.scalarop.rt = regno;
    __rmonWriteWordTo((u32*)SP_IMEM_START, inst.everything);
    __rmonWriteWordTo((u32*)SP_PC_REG, 0);
}

static void LoadStoreVU(int opcode, int regno) {
    INSTRUCTION inst;

    inst.everything = 0;
    inst.vectorop.opcode = opcode;
    inst.vectorop.rt = regno;
    inst.vectorop.size = 4; /* LQV / SQV */
    __rmonWriteWordTo((u32*)SP_IMEM_START, inst.everything);
    __rmonWriteWordTo((u32*)SP_PC_REG, 0);
}

static void SetUpForRCPop(int isVector) {
    RCPpc = __rmonReadWordAt((u32*)SP_PC_REG);
    oldIMEMvalue = __rmonReadWordAt((u32*)SP_IMEM_START);
    DMEMbuffer[0] = __rmonReadWordAt((u32*)SP_DMEM_START);
    if (isVector) {
        DMEMbuffer[1] = __rmonReadWordAt((u32*)(SP_DMEM_START + 0x4));
        DMEMbuffer[2] = __rmonReadWordAt((u32*)(SP_DMEM_START + 0x8));
        DMEMbuffer[3] = __rmonReadWordAt((u32*)(SP_DMEM_START + 0xC));
    }
}

static void CleanupFromRCPop(int isVector) {
    __rmonWriteWordTo((u32*)SP_DMEM_START, DMEMbuffer[0]);
    if (isVector) {
        __rmonWriteWordTo((u32*)(SP_DMEM_START + 0x4), DMEMbuffer[1]);
        __rmonWriteWordTo((u32*)(SP_DMEM_START + 0x8), DMEMbuffer[2]);
        // BUG: the last word is not restored properly
        __rmonWriteWordTo((u32*)(SP_DMEM_START + 0xC), DMEMbuffer[2]);
    }
    __rmonWriteWordTo((u32*)SP_IMEM_START, oldIMEMvalue);
    __rmonWriteWordTo((u32*)SP_PC_REG, RCPpc);
}

int __rmonGetGRegisters(KKHeader* req) {
    register KKObjectRequest* request = (KKObjectRequest*)req;
    KKGregEvent reply;

    ((void)"GetGRegisters\n");

    reply.tid = request->object;
    reply.header.code = request->header.code;
    reply.header.error = 0;

    if (request->header.method == RMON_CPU) {
        OSThread* tptr = __rmonGetTCB(request->object);
        u64* tcbregptr;
        register s32 i;
        
        if (tptr == NULL) {
            return -2;
        }

        for (i = 1, tcbregptr = &tptr->context.at; i < 26; i++, tcbregptr++) {
            reply.registers.gregs[i] = *tcbregptr;
        }
        for (i = 28, tcbregptr = &tptr->context.gp; i < 34; i++, tcbregptr++) {
            reply.registers.gregs[i] = *tcbregptr;
        }

        reply.registers.gregs[34] = tptr->context.cause;
        reply.registers.gregs[35] = tptr->context.pc;
        reply.registers.gregs[36] = tptr->context.sr;
        reply.registers.gregs[0] = 0;
    } else {
        return -2;
    }
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

int __rmonSetGRegisters(KKHeader* req) {    
    register KKGRegsetRequest* request = (KKGRegsetRequest*)req;
    KKObjectEvent reply;

    ((void)"SetGRegisters\n");

    if (request->header.method == RMON_CPU) {
        OSThread* tptr = __rmonGetTCB(request->tid);
        u64* tcbregptr;
        register int i;

        if (tptr == NULL) {
            return -2;
        }

        for (i = 1, tcbregptr = &tptr->context.at; i < 26; i++, tcbregptr++) {
            *tcbregptr = (s32)request->registers.gregs[i];
        }

        for (i = 28, tcbregptr = &tptr->context.gp; i < 34; i++, tcbregptr++) {
            *tcbregptr = (s32)request->registers.gregs[i];
        }

        tptr->context.cause = request->registers.gregs[34];
        tptr->context.pc = request->registers.gregs[35];
        tptr->context.sr = request->registers.gregs[36];
    } else {
        return -2;
    }

    reply.object = request->tid;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

int __rmonGetFRegisters(KKHeader* req) {
    register KKObjectRequest* request = (KKObjectRequest*)req;
    KKFPregEvent reply;
    OSThread* tptr;
    volatile float f;

    ((void)"GetFRegisters\n");

    if (req->method != RMON_CPU) {
        return -2;
    }

    /* touch fpu to ensure registers are saved to the context structure */
    f = 0.0f;

    tptr = __rmonGetTCB(request->object);
    if (tptr == NULL) {
        return -2;
    }

    __rmonCopyWords(reply.registers.fpregs.regs, (u32*)&tptr->context.fp0, 32);

    reply.registers.fpcsr = tptr->context.fpcsr;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    reply.tid = request->object;

    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;

}

int __rmonSetFRegisters(KKHeader* req) {
    register KKFPRegsetRequest* request = (KKFPRegsetRequest*)req;
    KKObjectEvent reply;
    OSThread* tptr;
    volatile float f;

    ((void)"SetFRegisters\n");

    if (req->method != RMON_CPU) {
        return -2;
    }

    f = 0.0f;

    tptr = __rmonGetTCB(request->tid);
    if (tptr == NULL) {
        return -2;
    }

    __rmonCopyWords((u32*)&tptr->context.fp0, request->registers.fpregs.regs, 32);
    tptr->context.fpcsr = request->registers.fpcsr;

    reply.object = request->tid;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

static u32 rmonGetRcpRegister(int regNumber) {
    u32 contents;

    if (__rmonRCPrunning()) {
        return 0;
    }

    SetUpForRCPop(FALSE);
    LoadStoreSU(43/* SW */, regNumber);
    __rmonStepRCP();
    contents = __rmonReadWordAt(SP_DMEM_START);
    CleanupFromRCPop(FALSE);
    return contents;

}

int __rmonGetSRegs(KKHeader* req) {
    register KKObjectRequest* request = (KKObjectRequest*)req;
    KKCpSregEvent reply;
    register int i;

    ((void)"GetSRegisters\n");

    if (__rmonRCPrunning()) {
        return -4;
    }

    reply.tid = request->object;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    SetUpForRCPop(FALSE);

    for (i = 0; i < 32; i++) {
        LoadStoreSU(43/* SW */, i);
        __rmonStepRCP();
        reply.registers.sregs[i] = __rmonReadWordAt(SP_DMEM_START);
    }
    CleanupFromRCPop(FALSE);
    reply.registers.sregs[32 + 0] = __rmonReadWordAt((u32*)SP_DRAM_ADDR_REG);
    reply.registers.sregs[32 + 1] = __rmonReadWordAt((u32*)SP_MEM_ADDR_REG);
    reply.registers.sregs[32 + 2] = __rmonReadWordAt((u32*)SP_RD_LEN_REG);
    reply.registers.sregs[32 + 3] = __rmonReadWordAt((u32*)SP_PC_REG) + SP_IMEM_START;
    reply.registers.sregs[32 + 4] = __rmonReadWordAt((u32*)SP_WR_LEN_REG);
    reply.registers.sregs[32 + 5] = __rmonReadWordAt((u32*)SP_STATUS_REG);
    reply.registers.sregs[32 + 6] = __rmonReadWordAt((u32*)SP_DMA_FULL_REG);
    reply.registers.sregs[32 + 7] = __rmonReadWordAt((u32*)SP_DMA_BUSY_REG);
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

int __rmonSetSRegs(KKHeader* req) {
    register KKCpScalarRegsetRequest* request = (KKCpScalarRegsetRequest*)req;
    KKObjectEvent reply;
    register int i;

    ((void)"SetSRegisters\n");

    if (__rmonRCPrunning()) {
        return -4;
    }

    SetUpForRCPop(FALSE);
    for (i = 0; i < 32; i++) {
        __rmonWriteWordTo(SP_DMEM_START, request->registers.sregs[i]);
        LoadStoreSU(35/* LW */, i);
        __rmonStepRCP();
    }
    CleanupFromRCPop(FALSE);

    __rmonWriteWordTo(SP_DRAM_ADDR_REG, request->registers.sregs[32 + 0]);
    __rmonWriteWordTo(SP_MEM_ADDR_REG, request->registers.sregs[32 + 1]);
    __rmonWriteWordTo(SP_PC_REG, request->registers.sregs[32 + 3] & 0xFFF);
    __rmonWriteWordTo(SP_WR_LEN_REG, request->registers.sregs[32 + 4]);
    __rmonWriteWordTo(SP_STATUS_REG, request->registers.sregs[32 + 5]);

    reply.object = request->tid;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

int __rmonGetVRegs(KKHeader* req) {
    char* cPtr;
    int sent;
    int dataSize;
    register KKObjectRequest* request = (KKHeader*)req;
    KKCpVregEvent reply;
    register int i;

    ((void)"GetVRegisters\n");

    if (__rmonRCPrunning()) {
        return -4;
    }

    reply.tid = request->object;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    reply.header.length = sizeof(reply);

    dataSize = sizeof(reply);
    cPtr = &dataSize;
    sent = 0;

    while (sent < 4) {
        sent += __osRdbSend(cPtr + sent, 4 - sent, 8);
    }

    __rmonSendHeader(&reply.header, 0x10, 1);

    SetUpForRCPop(TRUE);
    for (i = 0; i < 32; i++) {
        LoadStoreVU(58/* SWC2 */, i);
        __rmonStepRCP();
        __rmonSendData(SP_DMEM_START, 0x10);
    }
    CleanupFromRCPop(TRUE);

    return 0;
}

int __rmonSetVRegs(KKHeader* req) {
    register KKCpVectorRegsetRequest* request = (KKCpVectorRegsetRequest*)req;
    KKObjectEvent reply;
    register int i;

    ((void)"SetVRegs\n");

    if (__rmonRCPrunning()) {
        return -4;
    }

    SetUpForRCPop(TRUE);
    for (i = 0; i < 32; i++) {
        __rmonCopyWords(SP_DMEM_START, &request->registers.vregs[i], 4);
        LoadStoreVU(50/* LWC2 */, i);
        __rmonStepRCP();
    }
    CleanupFromRCPop(TRUE);

    reply.object = request->tid;
    reply.header.code = request->header.code;
    reply.header.error = 0;
    __rmonSendReply(&reply.header, sizeof(reply), 1);
    return 0;
}

u32 __rmonGetRegisterContents(int method, int threadNumber, int regNumber) {
    if (method == RMON_CPU) {
        // cpu register
        u32* regPointer;
        OSThread* tptr;

        if (regNumber > 0 && regNumber < 26) {
            regNumber -= 1;
        } else if (regNumber >= 28 && regNumber < 32) {
            regNumber -= 3;
        } else {
            return 0;
        }
        tptr = __rmonGetTCB(threadNumber);
        if (tptr == NULL) {
            return 0;
        }
        regPointer = (u32*)&tptr->context;
        regPointer += regNumber;
        return *regPointer;
    } else {
        // rsp register
        return rmonGetRcpRegister(regNumber);
    }
}

#endif
