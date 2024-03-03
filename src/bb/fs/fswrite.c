#include "PR/os_internal.h"
#include "PR/bbfs.h"
#include "PR/os_bbcard.h"

u16 __osBbFReallocBlock(BbInode* in, u16 block, u16 newVal) {
    u16 b;
    u16 ob;
    u16 prev;
    s32 incr;
    BbFat16* fat;

    prev = 0;
    fat = __osBbFat;

    if (in->size > 0x100000) {
        ob = 0x40;
        incr = 1;
    } else {
        incr = -1;
        ob = __osBbFsBlocks - 1;
    }

    if (ob >= __osBbFsBlocks) {
        return 0xFFFE;
    }

    while (ob < __osBbFsBlocks) {
        if (fat[ob / 0x1000].entry[ob % 0x1000] == 0) {
            break;
        }
        ob += incr;
    }

    if (ob < __osBbFsBlocks) {
        b = in->block;
        while (b != block) {
            prev = b;
            b = fat[b / 0x1000].entry[b % 0x1000];
        }
        if (prev != 0) {
            fat[prev / 0x1000].entry[prev % 0x1000] = ob;
        } else {
            in->block = ob;
        }

        fat[ob / 0x1000].entry[ob % 0x1000] = fat[b / 0x1000].entry[b % 0x1000];
        fat[b / 0x1000].entry[b % 0x1000] = newVal;
        if (__osBbFsSync(0) == 0) {
            return ob;
        }
    }

    return 0xFFFE;
}

s32 osBbFWrite(s32 fd, u32 off, void* buf, u32 len) {
    s32 rv;
    BbInode* in;
    u32 count;
    u32 b;
    u32 i;
    BbFat16* fat;
    u16 blocks[4];
    u16 n;

    if ((u32) fd >= 0x199U) {
        return -3;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;
    in = &fat->inode[fd];
    rv = -3;

    if ((in->type != 0) && !(off & 0x3FFF) && (off < in->size)) {
        if (!(len & 0x3FFF) && (off + len >= off) && (in->size >= off + len)) {
            if (len == 0) {
                rv = 0;
                goto end;
            }

            b = in->block;
            for (i = 0; i < off / 0x4000; i++) {
                b = fat[b / 0x1000].entry[b % 0x1000];
            }

            count = 0;

            while (len != 0) {
                for (n = 0; (len != 0) && (n < 4U); n++) {
                    if ((b == 0) || (b >= (u32) (__osBbFsBlocks - 0x10))) {
                        goto end;
                    }

                    blocks[n] = b;
                    b = fat[b / 0x1000].entry[b % 0x1000];

                    len = (len > 0x4000) ? (len - 0x4000) : 0;
                    count += 0x4000;
                }

                if (((rv = osBbCardEraseBlocks(0, blocks, n)) < 0) || (rv = osBbCardWriteBlocks(0, blocks, n, buf, NULL)) < 0) {
                    int i;

                    if (rv != -2) {
                        goto end;
                    }

                    for(i = 0; i < n; i++) {
                        u16 b = blocks[i];

                    retry:
                        if ((rv = osBbCardEraseBlock(0, b)) < 0 || ((rv = osBbCardWriteBlock(0, b, buf + i * 0x4000, NULL)) < 0)) {
                            if (rv != -2) {
                                goto end;
                            }
                            if ((b = __osBbFReallocBlock(in, b, 0xFFFE)) == 0xFFFE) {
                                goto end;
                            }
                            goto retry;
                        }
                    }
                }
                buf += n  * 0x4000;
            }
            rv = count;
        }
    }

end:
    __osBbFsRelAccess();
    return rv;
}