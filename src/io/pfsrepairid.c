#include "PR/os_internal.h"
#include "controller.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osPfsRepairId(OSPfs* pfs) {
    s32 ret = 0;

    if ((pfs->status & (PFS_INITIALIZED | PFS_ID_BROKEN))) {
        ret = __osGetId(pfs);

        if (ret == 0) {
            pfs->status &= ~(PFS_ID_BROKEN);
        }
    } else {
        ret = PFS_ERR_INVALID;
    }

    return ret;
}
