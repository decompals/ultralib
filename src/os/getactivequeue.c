#include "PR/os_internal.h"
#include "osint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

OSThread *__osGetActiveQueue(void) {
    return __osActiveQueue;
}
