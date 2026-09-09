#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif

// A stack frame was added to hardware interrupt handlers in 2.0J
#if BUILD_VERSION >= VERSION_J

struct __osHwInt {
    s32 (*handler)(void);
    void* stackEnd;
};

extern struct __osHwInt __osHwIntTable[];

void __osSetHWIntrRoutine(OSHWIntr interrupt, s32 (*handler)(void), void* stackEnd) {
    register u32 saveMask = __osDisableInt();

    __osHwIntTable[interrupt].handler = handler;
    __osHwIntTable[interrupt].stackEnd = stackEnd;
    __osRestoreInt(saveMask);
}

#else

#if !(defined(_DEBUG) && BUILD_VERSION <= VERSION_G)
extern struct s32 (*__osHwIntTable[])(void);
#endif

void __osSetHWIntrRoutine(OSHWIntr interrupt, s32 (*handler)(void)) {
    register u32 saveMask = __osDisableInt();

    __osHwIntTable[interrupt] = handler;
    __osRestoreInt(saveMask);
}

#endif
