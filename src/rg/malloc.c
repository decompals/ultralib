#include "PR/region.h"
#include "PR/ultraerror.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "PR/os.h"
#else
#include "PR/os_internal.h"
#endif
#include "assert.h"











































// TODO: this comes from a header
#ident "$Revision: 1.17 $"

void* osMalloc(void* region) {
    register OSRegion* rp = region;
    char* addr;

#ifdef _DEBUG
#if BUILD_VERSION <= VERSION_G
#line 55 "../rg/malloc.c"
#endif
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
