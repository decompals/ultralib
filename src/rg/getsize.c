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

s32 osGetRegionBufSize(void* region) {
    register OSRegion* rp = region;

#ifdef _DEBUG
#if BUILD_VERSION <= VERSION_G
#line 48 "../rg/getsize.c"
#endif
    assert(rp != NULL);
    if (((char*)rp + ALIGN(sizeof(OSRegion), rp->r_alignSize)) != rp->r_startBufferAddress) {
        __osError(ERR_OSGETREGIONBUFSIZE, 1, region);
        return 0;
    }
#endif

    return rp->r_bufferSize;
}
