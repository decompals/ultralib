#include "PR/region.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 osGetRegionBufCount(void *region) {
    register OSRegion *rp = region;
    return rp->r_bufferCount;
}

