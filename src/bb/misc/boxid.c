#include "PR/os_internal.h"
#include "PR/bcp.h"

u32 osBbGetBoxId(void) {
    return PI_GPIO_GET_BOXID(IO_READ(PI_GPIO_REG));
}
