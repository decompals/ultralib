#include "PR/os_internal.h"
#include "PR/bcp.h"

u32 osBbGetBoxId(void) {
    return IO_READ(PI_60_REG) >> 0x10;
}
