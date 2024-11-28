#include "PR/os_internal.h"
#include "PR/bbfs.h"
#include "PR/os_bbcard.h"
#include "macros.h"

u16 __osBbFReallocBlock(BbInode* in, u16 block, u16 newVal) {
    u16 b;
    u16 ob;
    u16 prev = BBFS_BLOCK_FREE;
    s32 incr;
    BbFat16* fat = __osBbFat;

    if (in->size > 64 * BB_FL_BLOCK_SIZE) {
        ob = BBFS_SKSA_LIMIT;
        incr = 1;
    } else {
        incr = -1;
        ob = __osBbFsBlocks - 1;
    }

    if (ob >= __osBbFsBlocks) {
        return BBFS_BLOCK_BAD;
    }

    while (ob < __osBbFsBlocks) {
        if (BBFS_NEXT_BLOCK(fat, ob) == BBFS_BLOCK_FREE) {
            break;
        }
        ob += incr;
    }

    if (ob < __osBbFsBlocks) {
        b = in->block;
        while (b != block) {
            prev = b;
            b = BBFS_NEXT_BLOCK(fat, b);
        }

        if (prev != BBFS_BLOCK_FREE) {
            BBFS_NEXT_BLOCK(fat, prev) = ob;
        } else {
            in->block = ob;
        }

        BBFS_NEXT_BLOCK(fat, ob) = BBFS_NEXT_BLOCK(fat, b);
        BBFS_NEXT_BLOCK(fat, b) = newVal;

        if (__osBbFsSync(FALSE) == 0) {
            return ob;
        }
    }
    return BBFS_BLOCK_BAD;
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

    if (fd < 0 || fd >= BB_INODE16_NUM) {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;
    in = &fat->inode[fd];
    rv = BBFS_ERR_INVALID;

    if (in->type == 0) {
        goto end;
    }
    if (off % BB_FL_BLOCK_SIZE != 0) {
        goto end;
    }
    if (off >= in->size) {
        goto end;
    }
    if (len % BB_FL_BLOCK_SIZE != 0) {
        goto end;
    }
    if (off + len < off) {
        goto end;
    }
    if (in->size < off + len) {
        goto end;
    }

    if (len == 0) {
        rv = 0;
        goto end;
    }

    b = in->block;
    for (i = 0; i < off / BB_FL_BLOCK_SIZE; i++) {
        b = BBFS_NEXT_BLOCK(fat, b);
    }

    count = 0;
    while (len != 0) {
        for (n = 0; len != 0 && n < ARRLEN(blocks); n++) {
            if (b == BBFS_BLOCK_FREE || b >= __osBbFsBlocks - BBFS_FAT_LIMIT) {
                goto end;
            }

            blocks[n] = b;
            b = BBFS_NEXT_BLOCK(fat, b);

            len = (len > BB_FL_BLOCK_SIZE) ? (len - BB_FL_BLOCK_SIZE) : 0;
            count += BB_FL_BLOCK_SIZE;
        }

        if ((rv = osBbCardEraseBlocks(0, blocks, n)) < 0 ||
            (rv = osBbCardWriteBlocks(0, blocks, n, buf, NULL)) < 0) {
            int i;

            if (rv != BBFS_ERR_FAIL) {
                goto end;
            }

            for (i = 0; i < n; i++) {
                u16 b = blocks[i];

            retry:
                if ((rv = osBbCardEraseBlock(0, b)) < 0 ||
                    (rv = osBbCardWriteBlock(0, b, buf + i * BB_FL_BLOCK_SIZE, NULL)) < 0) {
                    if (rv != BBFS_ERR_FAIL) {
                        goto end;
                    }
                    if ((b = __osBbFReallocBlock(in, b, BBFS_BLOCK_BAD)) == BBFS_BLOCK_BAD) {
                        goto end;
                    }
                    goto retry;
                }
            }
        }
        buf += n * BB_FL_BLOCK_SIZE;
    }
    rv = count;

end:
    __osBbFsRelAccess();
    return rv;
}
