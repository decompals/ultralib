#include "PR/os_internal.h"
#include "viint.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

__OSViContext* __osViGetCurrentContext(void) {
    return __osViCurr;
}
