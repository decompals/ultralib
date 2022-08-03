#include "PR/os_internal.h"
#include "PR/rcp.h"

u32 osBbGetBoxId(void) {
    return IO_READ(PI_BASE_REG + 0x60) >> 0x10;
}
