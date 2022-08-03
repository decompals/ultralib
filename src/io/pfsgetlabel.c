#include "PR/os_internal.h"
#include "controller.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osPfsGetLabel(OSPfs *pfs, u8 *label, int *len) {
    int i;
    
    if (label == NULL) {
        return PFS_ERR_INVALID;
    }
    
    PFS_CHECK_ID;

    for (i = 0; i < ARRLEN(pfs->label); i++) {
        if(*(pfs->label + i) == 0) {
            break;
        }

        *label++ = *(pfs->label + i);
    }

    *len = i;
    return 0;
}
