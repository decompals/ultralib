#include "macros.h"
#include "PR/os_internal.h"
#include "osint.h"

OSTime __osCurrentTime;
u32 __osBaseCounter;
u32 __osViIntrCount;
u32 __osTimerCounter;
OSTimer __osBaseTimer;
OSTimer* __osTimerList = &__osBaseTimer;

#ifndef _FINALROM
OSMesgQueue __osProfTimerQ ALIGNED(8);
OSProf* __osProfileList;
OSProf* __osProfileListEnd;
u32 __osProfileOverflowBin;
#endif

void __osTimerServicesInit(void) {
    __osCurrentTime = 0;
    __osBaseCounter = 0;
    __osViIntrCount = 0;
    __osTimerList->next = __osTimerList->prev = __osTimerList;
    __osTimerList->interval = __osTimerList->value = 0;
    __osTimerList->mq = NULL;
    __osTimerList->msg = 0;
}

void __osTimerInterrupt(void) {
    OSTimer* t;
    u32 count;
    u32 elapsed_cycles;

#ifndef _FINALROM
    u32 pc;
    s32 offset;
    OSProf* prof = __osProfileList;
#endif

    if (__osTimerList->next == __osTimerList) {
        return;
    }
    for (;;) {
        t = __osTimerList->next;

        if (t == __osTimerList) {
            __osSetCompare(0);
            __osTimerCounter = 0;
            break;
        }

        count = osGetCount();
        elapsed_cycles = count - __osTimerCounter;
        __osTimerCounter = count;

        if (elapsed_cycles < t->value) {
            t->value -= elapsed_cycles;
            __osSetTimerIntr(t->value);
            break;
        }

        t->prev->next = t->next;
        t->next->prev = t->prev;
        t->next = NULL;
        t->prev = NULL;

        if (t->mq != NULL) {
#ifdef _FINALROM
            osSendMesg(t->mq, t->msg, OS_MESG_NOBLOCK);
        }
#else
            if (t->mq != &__osProfTimerQ) {
                osSendMesg(t->mq, t->msg, OS_MESG_NOBLOCK);
            } else {
                pc = __osRunQueue->context.pc;
                prof = __osProfileList;

                while (prof < __osProfileListEnd) {
                    offset = pc - (u32)prof->text_start;

                    if (offset >= 0) {
                        if ((s32)prof->text_end - (s32)pc > 0) {
                            prof->histo_base[offset >> 2]++;
                            goto __ProfDone;
                        }
                    }
                    prof++;
                }

                __osProfileOverflowBin++;
            }
        }
#endif

    __ProfDone:

        if (t->interval != 0) {
            t->value = t->interval;
            __osInsertTimer(t);
        }
    }
}

void __osSetTimerIntr(OSTime tim) {
    OSTime NewTime;
    u32 savedMask;

    if (tim < 468) {
        tim = 468;
    }

    savedMask = __osDisableInt();
    __osTimerCounter = osGetCount();
    NewTime = __osTimerCounter + tim;
    __osSetCompare(NewTime);
    __osRestoreInt(savedMask);
}

OSTime __osInsertTimer(OSTimer* t) {
    OSTimer* timep;
    OSTime tim;
    u32 savedMask;
    savedMask = __osDisableInt();

    timep = __osTimerList->next;
    tim = t->value;

    while (timep != __osTimerList && tim > timep->value) {
        tim -= timep->value;
        timep = timep->next;
    }

    t->value = tim;

    if (timep != __osTimerList) {
        timep->value -= tim;
    }

    t->next = timep;
    t->prev = timep->prev;
    timep->prev->next = t;
    timep->prev = t;
    __osRestoreInt(savedMask);
    return tim;
}