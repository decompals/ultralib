#include "PR/os_internal.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

void osDpGetCounters(u32* array) {
    *array++ = IO_READ(DPC_CLOCK_REG);
    *array++ = IO_READ(DPC_BUFBUSY_REG);
    *array++ = IO_READ(DPC_PIPEBUSY_REG);
    *array++ = IO_READ(DPC_TMEM_REG);
}
