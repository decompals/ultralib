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

void* osMalloc(void* region) {
    register OSRegion* rp = region;
    char* addr;

#ifdef _DEBUG
#line 56
    assert(rp != NULL);
    if (((char*)rp + ALIGN(sizeof(OSRegion), rp->r_alignSize)) != rp->r_startBufferAddress) {
        __osError(ERR_OSMALLOC, 1, region);
        return NULL;
    }
#endif

    if (rp->r_freeList == MAX_BUFCOUNT) {
        return NULL;
    }

    addr = &rp->r_startBufferAddress[rp->r_freeList * rp->r_bufferSize];
    rp->r_freeList = *(u16*)addr;
    return addr;
}
