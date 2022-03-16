#ifndef _RMONINT_H
#define _RMONINT_H

#include "dbgproto.h"
#include "PR/os_internal.h"

#define MIPS_BREAK_OPCODE 0xD
#define MIPS_BREAK_MASK 0xFC00003F

#define MIPS_BREAK(code) \
    ((((code) & 0xFFFFF) << 6) | MIPS_BREAK_OPCODE)

void __rmonIOhandler(void);

void __rmonWriteWordTo(u32* addr, u32 val);
u32 __rmonReadWordAt(u32* addr);

void __rmonCopyWords(u32* dest, u32* srce, u32 count);

OSThread* __rmonGetTCB(int threadNumber);

extern u8 __rmonUtilityBuffer[256];
extern u8 __rmonRcpAtBreak;
extern OSMesgQueue __rmonMQ;

#define RMON_RUNTHREAD_SSTEP (1 << 0)
#define RMON_RUNTHREAD_SETPC (1 << 1)

#define RMON_MESG_CPU_BREAK 2
#define RMON_MESG_SP_BREAK 4
#define RMON_MESG_FAULT 8

#define RMON_CPU 0
#define RMON_RSP 1

/* "thread id" for rsp */
#define RMON_TID_RSP 1000

#define RMON_PID_CPU 1002
#define RMON_PID_RSP 1001

typedef int (*FUNPTR)();

int __rmonLoadProgram(KKHeader* req);
int __rmonListProcesses(KKHeader* req);
int __rmonGetExeName(KKHeader* req);
int __rmonListThreads(KKHeader* req);
int __rmonThreadStatus(KKHeader* req);
int __rmonStopThread(KKHeader* req);
int __rmonRunThread(KKHeader* req);
int __rmonSetFault(KKHeader* req);
int __rmonGetRegionCount(KKHeader* req);
int __rmonGetRegions(KKHeader* req);
int __rmonGetGRegisters(KKHeader* req);
int __rmonSetGRegisters(KKHeader* req);
int __rmonGetFRegisters(KKHeader* req);
int __rmonSetFRegisters(KKHeader* req);
int __rmonReadMem(KKHeader* req);
int __rmonWriteMem(KKHeader* req);
int __rmonSetBreak(KKHeader* req);
int __rmonClearBreak(KKHeader* req);
int __rmonListBreak(KKHeader* req);
int __rmonSetComm(KKHeader* req);
int __rmonGetSRegs(KKHeader* req);
int __rmonSetSRegs(KKHeader* req);
int __rmonGetVRegs(KKHeader* req);
int __rmonSetVRegs(KKHeader* req);

#endif
