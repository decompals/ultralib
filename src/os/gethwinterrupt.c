#include "PR/os_internal.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

struct __osHwInt {
    s32 (*handler)(void);
#if BUILD_VERSION >= VERSION_J
    void* stackEnd;
#endif
};

extern struct __osHwInt __osHwIntTable[];

#if BUILD_VERSION >= VERSION_J
void __osGetHWIntrRoutine(OSHWIntr interrupt, s32 (**handler)(void), void** stackEnd) {
#else
void __osGetHWIntrRoutine(OSHWIntr interrupt, s32 (**handler)(void)) {
#endif
    *handler = __osHwIntTable[interrupt].handler;
#if BUILD_VERSION >= VERSION_J
    *stackEnd = __osHwIntTable[interrupt].stackEnd;
#endif
}
