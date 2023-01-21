#include "PR/os_internal.h"
#include "PR/bcp.h"

void osBbPowerOn(void) {
    // Power control = 1, Power mask = 1
    IO_WRITE(PI_60_REG, 0x11);
}

void osBbPowerOff(void) {
    // Power control = 0, Power mask = 1
    IO_WRITE(PI_60_REG, 0x10);
}
