#include "PR/os_internal.h"
#include "PR/ultraerror.h"
#include "PR/rcp.h"
#include "assert.h"
#include "viint.h"

extern u32 __osBbIsBb;

// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

void osViSetMode(OSViMode* modep) {
    register u32 saveMask;

#ifdef _DEBUG
    if (!__osViDevMgr.active) {
        __osError(ERR_OSVISETMODE, 0);
        return;
    }

#line 61
    assert(modep != NULL);
#endif

    saveMask = __osDisableInt();

#ifdef BBPLAYER
    if (__osBbIsBb) {
        modep->comRegs.ctrl &= ~VI_CTRL_PIXEL_ADV_2;
    }
#endif

    __osViNext->modep = modep;
    __osViNext->state = VI_STATE_MODE_UPDATED;
    __osViNext->control = __osViNext->modep->comRegs.ctrl;
    __osRestoreInt(saveMask);
}
