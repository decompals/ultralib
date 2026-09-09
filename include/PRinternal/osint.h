#ifndef _OSINT_H
#define _OSINT_H
#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif
#ifndef _IDO_G_OS_INTERNAL_COMPAT_H
typedef struct __OSEventState
{
    OSMesgQueue *messageQueue;
    OSMesg message;
} __OSEventState;
extern struct __osThreadTail
{
    OSThread *next;
    OSPri priority;
} __osThreadTail;
#endif

//maybe should be in exceptasm.h?
extern void __osEnqueueAndYield(OSThread **);
extern void __osDequeueThread(OSThread **, OSThread *);
extern void __osEnqueueThread(OSThread **, OSThread *);
extern OSThread *__osPopThread(OSThread **);
extern void __osDispatchThread(void);
extern void __osCleanupThread(void);

extern void __osSetTimerIntr(OSTime);
extern OSTime __osInsertTimer(OSTimer *);
extern void __osTimerInterrupt(void);
extern u32 __osProbeTLB(void *);
extern int     __osSpDeviceBusy(void);

extern OSThread *__osRunningThread;
extern OSThread *__osActiveQueue;
extern OSThread *__osFaultedThread;
extern OSThread *__osRunQueue;

extern OSTimer *__osTimerList;
#ifndef _IDO_G_OS_INTERNAL_COMPAT_H
extern OSTimer __osBaseTimer;
#endif
extern OSTime __osCurrentTime;
extern u32 __osBaseCounter;
extern u32 __osViIntrCount;
extern u32 __osTimerCounter;
#ifndef _IDO_G_OS_INTERNAL_COMPAT_H
extern u32 __osShutdown;
#endif

extern OSMesgQueue __osProfTimerQ;
extern OSProf *__osProfileList;
extern OSProf *__osProfileListEnd;
extern u32 __osProfileOverflowBin;

extern __OSEventState __osEventStateTab[];


extern void __osTimerServicesInit(void);
extern s32 __osAiDeviceBusy(void);
extern int __osDpDeviceBusy(void);

#if !defined(_FINALROM) && !defined(_IDO_G_OS_INTERNAL_COMPAT_H)
extern void* __printfunc;
#endif

#endif
