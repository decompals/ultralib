#include "PR/os_internal.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void osSpTaskYield(void) {
    __osSpSetStatus(SP_SET_YIELD);
}
