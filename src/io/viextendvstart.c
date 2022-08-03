#include <PR/os_internal.h>
#include "viint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void osViExtendVStart(u32 value) {
    __additional_scanline = value;
}
