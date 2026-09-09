#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif
#include "PR/ultraerror.h"
#include "assert.h"
#include "PRinternal/viint.h"









































// TODO: this comes from a header
#ident "$Revision: 1.17 $"

void osViSetMode(OSViMode* modep) {
    register u32 saveMask;

#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(ERR_OSVISETMODE, 0);
        return;
    }

#if BUILD_VERSION <= VERSION_G
#line 57 "../io/visetmode.c"
#endif
    assert(modep != NULL);
#endif

    saveMask = __osDisableInt();

    __osViNext->modep = modep;
    __osViNext->state = VI_STATE_MODE_UPDATED;
    __osViNext->control = __osViNext->modep->comRegs.ctrl;
    __osRestoreInt(saveMask);
}
