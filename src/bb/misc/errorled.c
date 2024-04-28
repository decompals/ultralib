#include "PR/os_internal.h"
#include "PR/bcp.h"

void osBbSetErrorLed(u32 value) {
    u32 mask = IO_READ(PI_GPIO_REG);
    mask &= ~PI_GPIO_LED_OFF;
    mask &= ~PI_GPIO_MASK_LED;
    IO_WRITE(PI_GPIO_REG, mask | ((value == 0) ? PI_GPIO_LED_OFF : PI_GPIO_LED_ON) | PI_GPIO_MASK_LED);
}

u32 osBbGetErrorLed(void) {
    u32 v = PI_GPIO_GET_LED(IO_READ(PI_GPIO_REG));
    return v;
}
