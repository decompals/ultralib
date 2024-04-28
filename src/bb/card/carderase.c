#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "bcp.h"

void __osBbCardRelAccess(void);
s32 __osBbCardGetAccess(void);
s32 __osBbCardWaitEvent(void);
s32 __osBbCardStatus(u32 dev, u8* status, u32 buf);

extern u8 __osBbCardMultiplane;

static void erase_block(u32 dev, u32 addr) {
    IO_WRITE(PI_70_REG, NAND_BLOCK_TO_ADDR(addr));
    IO_WRITE(PI_48_REG, NAND_ERASE_PAGE(dev, FALSE));

    do {
        if (IO_READ(MI_38_REG) & 0x02000000) {
            return;
        }
    } while (IO_READ(PI_48_REG) & NAND_STATUS_BUSY);

    IO_WRITE(PI_48_REG, NAND_EXEC_ERASE(dev, TRUE));
}

static void erase_blocks(u32 dev, u16* addr, u32 n) {
    do {
        IO_WRITE(PI_70_REG, NAND_BLOCK_TO_ADDR(addr[--n]));
        IO_WRITE(PI_48_REG, NAND_ERASE_PAGE(dev, FALSE));

        do {
            if (IO_READ(MI_38_REG) & 0x02000000) {
                return;
            }
        } while (IO_READ(PI_48_REG) & NAND_STATUS_BUSY);
    } while (n != 0);

    IO_WRITE(PI_48_REG, NAND_EXEC_ERASE(dev, TRUE));
}

s32 osBbCardEraseBlock(u32 dev, u16 block) {
    s32 rv;
    u8 tmp;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    erase_block(dev, block);
    rv = __osBbCardWaitEvent();
    if (rv < 0 || __osBbCardStatus(dev, &tmp, 0) != 0) {
        rv = BBCARD_ERR_CHANGED;
    } else if (tmp != NAND_RSTAT_ERASE_OK) {
        rv = BBCARD_ERR_FAIL;
    }
    __osBbCardRelAccess();
    return rv;
}

s32 osBbCardEraseBlocks(u32 dev, u16* block, u32 n) {
    u32 i = 0;
    s32 rv;
    u8 tmp;

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    while (i < n) {
        int j = i + 1;
        u32 plane = 1 << (block[i] & 3);
        u32 seg = block[i] >> 0xC;

        while (__osBbCardMultiplane && j < n) {
            int p = 1 << (block[j] & 3);

            if ((p & plane) || seg != (block[j] >> 0xC)) {
                break;
            }
            plane |= p;
            j++;
        }
    
        erase_blocks(dev, &block[i], j - i);

        rv = __osBbCardWaitEvent();
        if (rv < 0) {
            goto err;
        }
        i = j;
    }

    if (__osBbCardStatus(dev, &tmp, 0) != 0) {
        rv = BBCARD_ERR_CHANGED;
    } else if (tmp != NAND_RSTAT_ERASE_OK) {
        rv = BBCARD_ERR_FAIL;
    }
err:
    __osBbCardRelAccess();
    return rv;
}
