#include "PR/os_internal.h"
#include "bcp.h"

void __osBbCardDmaCopy(u32 which, void* addr, u32 dir);
void __osBbCardRelAccess(void);
s32 __osBbCardGetAccess(void);
s32 __osBbCardWaitEvent(void);
s32 __osBbCardStatus(u32 dev, u8* status, u32 buf);

extern u8 __osBbCardMultiplane;

u8 __osBbCardNoEcc;

static void fill_page(unsigned int dev, unsigned int addr, int which_buf, int wait) {
    IO_WRITE(PI_70_REG, addr << 9);

#ifndef _DEBUG
    IO_WRITE(PI_48_REG, 0xAF800000 | (which_buf << 0xE) | (dev << 0xC) | 0xE10);
#else
    IO_WRITE(PI_48_REG, 0xAF800000 | (which_buf << 0xE) | (dev << 0xC) | (((addr & 0x1F) == 0x1F && __osBbCardNoEcc) ? 0 : 0x800) | 0x610);
    if ((__osBbCardNoEcc != 0) && (addr & 0x1F) == 0x1F) {
        osSyncPrintf("write no ecc\n");
    }
#endif

    if (wait) {
        do {
            if (IO_READ(MI_38_REG) & 0x02000000) {
                return;
            }
        } while (IO_READ(PI_48_REG) & 0x80000000);
    }
}

static void write_page(unsigned int dev) {
    IO_WRITE(PI_48_REG, 0xC0108000 | (dev << 0xC));

    do {
        if (IO_READ(MI_38_REG) & 0x02000000) {
            return;
        }
    } while (IO_READ(PI_48_REG) & 0x80000000);
}

static void write_dummy(u32 dev) {
    IO_WRITE(PI_48_REG, 0x80118000 | (dev << 0xC));

    do {
        if (IO_READ(MI_38_REG) & 0x02000000) {
            return;
        }
    } while (IO_READ(PI_48_REG) & 0x80000000);
}

s32 osBbCardWriteBlock(u32 dev, u16 block, void* addr, void* spare) {
    u32 i;
    u32 b = 0;
    s32 rv;
    u8 tmp;

#ifdef _DEBUG
    if (((u32)addr & 0xF) != 0) {
        return -3;
    }
#endif

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    if (spare != NULL) {
        u8* var_t1 = spare;

        for (i = 0; i < 0x10; i += 4) {
            u32 x = (var_t1[0] << 0x18) | (var_t1[1] << 0x10) | (var_t1[2] << 8) | var_t1[3];

            IO_WRITE(PI_BASE_REG + 0x10400 + i, x);
            IO_WRITE(PI_BASE_REG + 0x10410 + i, x);

            var_t1 += 4;
        }

    } else {
        for (i = 0; i < 0x10; i += 4) {
            IO_WRITE(PI_BASE_REG + 0x10400 + i, 0xFFFFFFFF);
            IO_WRITE(PI_BASE_REG + 0x10410 + i, 0xFFFFFFFF);
        }
    }

    __osBbCardDmaCopy(b, addr, 1);

    for (i = 0; i < 0x20; i++) {
        fill_page(dev, (block * 0x4000 + i * 0x200) >> 9, b, 1);
        write_page(dev);

        if (i < 0x1F) {
            __osBbCardDmaCopy(b ^ 1, &((u8*)addr)[0x200 * (i + 1)], 1);
        }

        rv = __osBbCardWaitEvent();
        if (rv < 0 || __osBbCardStatus(dev, &tmp, b) != 0) {
            goto err;
        }
        if (tmp != 0xC0) {
#ifdef _DEBUG
            osSyncPrintf("block %d write failed 0x%x\n", block, tmp);
#endif
            rv = -2;
            goto err;
        }
        b ^= 1;
    }

err:
    __osBbCardRelAccess();
    return rv;
}

s32 osBbCardWriteBlocks(u32 dev, u16* block, u32 n, void* addr, void* spare) {
    u32 i;
    u32 j;
    u32 b = 0;
    s32 rv;
    u8 tmp;

#ifdef _DEBUG
    if (((u32)addr & 0xF) != 0) {
        return -3;
    }
#endif

    rv = __osBbCardGetAccess();
    if (rv < 0) {
        return rv;
    }

    if (spare != NULL) {
        u8* p = spare;

        for (i = 0; i < 0x10; i += 4) {
            u32 x = (p[0] << 0x18) | (p[1] << 0x10) | (p[2] << 8) | p[3];
            IO_WRITE(PI_BASE_REG + 0x10400 + i, x);
            IO_WRITE(PI_BASE_REG + 0x10410 + i, x);
            p += 4;
        }
    } else {
        for (i = 0; i < 0x10; i += 4) {
            IO_WRITE(PI_BASE_REG + 0x10400 + i, 0xFFFFFFFF);
            IO_WRITE(PI_BASE_REG + 0x10410 + i, 0xFFFFFFFF);
        }
    }

    j = 0;
    while (j < n) {
        u32 plane = 1 << (block[j] & 3);
        u32 seg = block[j] >> 0xC;
        int l;
        int k = j + 1;

        while (__osBbCardMultiplane && k < n) {
            if (((1 << (block[k] & 3)) & plane) || seg != (block[k] >> 0xC)) {
                break;
            }
            plane |= 1 << (block[k] & 3);
            k++;
        }

        for (i = 0; i < 0x20; i++) {
            __osBbCardDmaCopy(b, addr + j * 0x4000 + i * 0x200, 1);

            for(l = j; l < k; l++) {
                fill_page(dev, ((block[l] * 0x4000) + (i * 0x200)) >> 9, (s32) b, 0);
                if (l < k - 1) {
                    __osBbCardDmaCopy(b ^ 1, addr + ((l + 1) * 0x4000) + (i * 0x200), 1);
                }

                do {
                    if (IO_READ(MI_38_REG) & 0x02000000) {
                        break;
                    }
                } while (IO_READ(PI_48_REG) & 0x80000000);

                if (l == k - 1) {
                    write_page(dev);
                    rv = __osBbCardWaitEvent();
                    if (rv < 0) {
                        goto err;
                    }
                } else {
                    write_dummy(dev);
                }

                b ^= 1;
            }

            if (__osBbCardStatus(dev, &tmp, 0) != 0) {
                rv = -4;
                goto err;
            }
            if (tmp != 0xC0) {
                rv = -2;
                goto err;
            }
        }
        j = k;
    }

err:
    __osBbCardRelAccess();
    return rv;
}
