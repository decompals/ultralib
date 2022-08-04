#include "PR/os_internal.h"
#include "osint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

OSThread *__osGetNextFaultedThread(OSThread *lastFault) {
    register int saveMask = __osDisableInt();
    register OSThread *fault;

    fault = (lastFault == NULL) ? __osActiveQueue : lastFault;

    while (fault->priority != -1) {
        if ((fault->flags & OS_FLAG_FAULT) != 0 && fault != lastFault) {
            break;
        }
        fault = fault->tlnext;
    }

    if (fault->priority == -1) {
        fault = NULL;
    }

    __osRestoreInt(saveMask);
    return fault;
}

