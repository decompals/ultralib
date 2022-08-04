#include "PR/os_internal.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

struct __osHwInt {
    s32 (*handler)(void);
    void *stackEnd;
};

extern struct __osHwInt __osHwIntTable[];

void __osGetHWIntrRoutine(OSHWIntr interrupt, s32 (**handler)(void), void **stackEnd) {
    *handler = __osHwIntTable[interrupt].handler;
    *stackEnd = __osHwIntTable[interrupt].stackEnd;
}
