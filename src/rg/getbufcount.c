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

s32 osGetRegionBufCount(void* region) {
    register OSRegion* rp = region;

#ifdef _DEBUG
#if BUILD_VERSION <= VERSION_G
#line 49 "../rg/getbufcount.c"
#endif
    assert(rp != NULL);
    if (((char*)rp + ALIGN(sizeof(OSRegion), rp->r_alignSize)) != rp->r_startBufferAddress) {
        __osError(ERR_OSGETREGIONBUFCOUNT, 1, region);
        return 0;
    }
#endif
    return rp->r_bufferCount;
}
