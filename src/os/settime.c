#include "PR/os_internal.h"
#include "PR/ultraerror.h"
#include "osint.h"
#include "../io/viint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void osSetTime(OSTime time) {

#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(ERR_OSSETTIME, 0);
        return;
    }
#endif

    __osCurrentTime = time;
}
