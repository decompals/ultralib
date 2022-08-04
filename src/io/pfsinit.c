#include "PR/os_internal.h"
#include "controller.h"
#include "siint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osPfsInit(OSMesgQueue *queue, OSPfs *pfs, int channel) {
    s32 ret = 0;
    
    __osSiGetAccess();
    ret = __osPfsGetStatus(queue, channel);
    __osSiRelAccess();
    
    if (ret != 0) {
        return ret;
    }

    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;
    pfs->activebank = -1;
    ERRCK(__osGetId(pfs));

    ret = osPfsChecker(pfs);
    pfs->status |= PFS_INITIALIZED;
    return ret;
}
