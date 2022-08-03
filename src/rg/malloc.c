#include "PR/region.h"

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void *osMalloc(void *region) {
    register OSRegion *rp = region;
    unsigned char *addr;

    if (rp->r_freeList == MAX_BUFCOUNT) {
        return NULL;
    }
    
    addr = &rp->r_startBufferAddress[rp->r_freeList * rp->r_bufferSize];
    rp->r_freeList = *(u16 *)addr;
    return addr;
}
