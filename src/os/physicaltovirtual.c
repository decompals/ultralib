#include "PR/os_internal.h"
#include "PR/R4300.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void* osPhysicalToVirtual(u32 addr) {
    return (void*)PHYS_TO_K0(addr);
}
