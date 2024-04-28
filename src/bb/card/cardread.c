#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "bcp.h"

void __osBbCardDmaCopy(u32 which, void* addr, u32 dir);
void __osBbCardRelAccess(void);
s32 __osBbCardGetAccess(void);
s32 __osBbCardWaitEvent(void);

u32 __osBbCardSbErr;

static void read_page(u32 dev, u32 addr, u32 which_buf) {
    IO_WRITE(PI_70_REG, NAND_PAGE_TO_ADDR(addr));
    IO_WRITE(PI_48_REG, NAND_READ_0(0x210, which_buf, dev, TRUE, TRUE));
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

    for (i = 0; i < NAND_PAGES_PER_BLOCK; i++) {
        read_page(dev, (block * NAND_BYTES_PER_BLOCK + i * NAND_BYTES_PER_PAGE) / NAND_BYTES_PER_PAGE, b);

        if (i != 0) {
            __osBbCardDmaCopy(b ^ 1, &((u8*)addr)[NAND_BYTES_PER_PAGE * (i - 1)], 0);

            if (spare != NULL) {
                u32 addr = PI_NAND_SPARE_BUFFER(b ^ 1, 0);

                ((u8*)spare)[5] &= IO_READ(addr + 4) >> 8;
            }
        }

        rv = __osBbCardWaitEvent();
        if (rv < 0) {
            goto err;
        }

        x = IO_READ(PI_48_REG);
        if (x & NAND_STATUS_ERROR_DB) {
            db = BBCARD_ERR_FAIL;

            if (spare == NULL || i == (NAND_PAGES_PER_BLOCK - 1)) {
                goto err;
            }
        }

        if (x & NAND_STATUS_ERROR_SB) {
            __osBbCardSbErr++;
        }
        b ^= 1;
    }

    __osBbCardDmaCopy(b ^ 1, addr + NAND_BYTES_PER_PAGE * (i - 1), 0);

err:
    if (spare != NULL) {
        u8* p = spare;
        u32 addr = PI_NAND_SPARE_BUFFER(b ^ 1, 0);

        for (i = 0; i < NAND_PAGE_SPARE_SIZE/sizeof(u32); i++) {
            u32 x = IO_READ(addr + sizeof(u32) * i);

            p[0] = x >> 0x18;
            p[1] = x >> 0x10;
            if (i == 1) {
                p[2] &= x >> 8;
            } else {
                p[2] = x >> 8;
            }
            p[3] = x;

            p += sizeof(u32);
        }
    }

    __osBbCardRelAccess();
    return (db != 0) ? db : rv;
}
