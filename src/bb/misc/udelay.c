#include "PR/os_internal.h"

void __osBbDelay(u32 usec) {
    u32 start = osGetCount();
    u32 count;
    u32 sum = 0;
    u32 cycles = OS_USEC_TO_CYCLES(usec);

    do {
        count = osGetCount();
        if (count >= start) {
            sum += count - start;
        } else {
            sum += count - (start + 1) + 1;
        }
        start = count;
    } while (sum < cycles);
}
