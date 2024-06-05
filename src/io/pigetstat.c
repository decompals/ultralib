#include "PR/os_internal.h"
#include "PR/piint.h"

// TODO: this comes from a header
#ident "$Revision: 1.17 $"

u32 osPiGetStatus() {
    return IO_READ(PI_STATUS_REG);
}
