#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#else
#include "PR/os_internal.h"
#endif
#include "PR/R4300.h"

void* osPhysicalToVirtual(u32 addr) {
    return (void*)PHYS_TO_K0(addr);
}
