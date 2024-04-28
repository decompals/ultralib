#include "PR/os_internal.h"
#include "PR/os_bbcard.h"
#include "bcp.h"

void __osBbCardDmaCopy(u32 which, void* addr, u32 dir);
void __osBbCardRelAccess(void);
s32 __osBbCardGetAccess(void);
s32 __osBbCardWaitEvent(void);

u32 __osBbCardSbErr;

static void read_page(u32 dev, u32 addr, u32 which_buf) {
    IO_WRITE(PI_70_REG, addr << 9);
    IO_WRITE(PI_48_REG, 0xDF008000 | (which_buf << 0xE) | (dev << 0xC) | 0xA10);
}

s32 osBbCardReadBlock(u32 dev, u16 block, void* addr, void* spare) {
    u32 x;
    u32 i;
    u32 b = 0;
    s32 rv;
    s32 db = 0;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    if (spare != NULL) {
        ((u8*)spare)[5] = 0xFF;
    }

    for (i = 0; i < 0x20; i++) {
        read_page(dev, (block * 0x4000 + i * 0x200) >> 9, b);

        if (i != 0) {
            __osBbCardDmaCopy(b ^ 1, &((u8*)addr)[0x200 * (i - 1)], 0);

            if (spare != NULL) {
                u32 addr = PI_BASE_REG + 0x10400 + (b ^ 1) * 0x10;

                ((u8*)spare)[5] &= IO_READ(addr + 4) >> 8;
            }
        }

        rv = __osBbCardWaitEvent();
        if (rv < 0) {
            goto err;
        }

        x = IO_READ(PI_48_REG);
        if (x & 0x400) {
            db = -2;

            if (spare == NULL || i == 0x1F) {
                goto err;
            }
        }

        if (x & 0x800) {
            __osBbCardSbErr++;
        }
        b ^= 1;
    }

    __osBbCardDmaCopy(b ^ 1, addr + 0x200 * (i - 1), 0);

err:
    if (spare != NULL) {
        u8* p = spare;
        u32 addr = PI_BASE_REG + 0x10400 + (b ^ 1) * 0x10;

        for (i = 0; i < 4; i++) {
            u32 x = IO_READ(addr + 4 * i);

            p[0] = x >> 0x18;
            p[1] = x >> 0x10;
            if (i == 1) {
                p[2] &= x >> 8;
            } else {
                p[2] = x >> 8;
            }
            p[3] = x;

            p += 4;
        }
    }

    __osBbCardRelAccess();
    return (db != 0) ? db : rv;
}
