#include "PR/region.h"
#include "PR/ultraerror.h"
#include "PR/os_internal.h"
#include "assert.h"





































// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 osGetRegionBufSize(void* region) {
    register OSRegion* rp = region;

#ifdef _DEBUG
    assert(rp != NULL);
    if (((char*)rp + ALIGN(sizeof(OSRegion), rp->r_alignSize)) != rp->r_startBufferAddress) {
        __osError(ERR_OSGETREGIONBUFSIZE, 1, region);
        return 0;
    }
#endif

    return rp->r_bufferSize;
}
