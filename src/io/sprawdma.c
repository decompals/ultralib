#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "../os/osint.h"
#include "assert.h"


















































// TODO: this comes from a header
#ifndef BBPLAYER
#ident "$Revision: 1.17 $"
#else
#ident "$Revision: 1.1 $"
#endif

s32 __osSpRawStartDma(s32 direction, u32 devAddr, void* dramAddr, u32 size) {
    assert(((u32)devAddr & 0x7) == 0);
    assert(((u32)dramAddr & 0x7) == 0);
    assert(((u32)size & 0x7) == 0);

    if (__osSpDeviceBusy()) {
        return -1;
    }

    IO_WRITE(SP_MEM_ADDR_REG, devAddr);
    IO_WRITE(SP_DRAM_ADDR_REG, osVirtualToPhysical(dramAddr));

    if (direction == OS_READ) {
        IO_WRITE(SP_WR_LEN_REG, size - 1);
    } else {
        IO_WRITE(SP_RD_LEN_REG, size - 1);
    }

    return 0;
}
