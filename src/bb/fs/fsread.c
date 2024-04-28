#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "PR/bbfs.h"

s32 osBbFRead(s32 fd, u32 off, void* buf, u32 len) {
    s32 rv;
    BbInode* in;
    u32 count;
    u32 b;
    u32 i;
    BbFat16* fat;

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
    if (off >= in->size || off + len < off) {
        goto end;
    }

    if (in->size < off + len) {
        len = in->size - off;
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
        if (b == BBFS_BLOCK_FREE || b >= __osBbFsBlocks - BBFS_FAT_LIMIT) {
            goto end;
        }

        rv = osBbCardReadBlock(0, b, buf, NULL);
        if (rv < 0) {
            goto end;
        }

        b = BBFS_NEXT_BLOCK(fat, b);

        buf += BB_FL_BLOCK_SIZE;
        len = (len > BB_FL_BLOCK_SIZE) ? (len - BB_FL_BLOCK_SIZE) : 0;
        count += BB_FL_BLOCK_SIZE;
    }
    rv = count;

end:
    __osBbFsRelAccess();
    return rv;
}
