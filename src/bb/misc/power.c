#include "PR/os_internal.h"
#include "PR/bcp.h"

void osBbPowerOn(void) {
    IO_WRITE(PI_GPIO_REG, PI_GPIO_MASK_PWR | PI_GPIO_PWR_ON);
}

void osBbPowerOff(void) {
    IO_WRITE(PI_GPIO_REG, PI_GPIO_MASK_PWR | PI_GPIO_PWR_OFF);
}
