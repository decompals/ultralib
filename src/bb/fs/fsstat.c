#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFStat(s32 fd, OSBbStatBuf* sb, u16* blockList, u32 listLen) {
    int i;
    s32 rv;
    BbInode* in;
    BbFat16* fat;

    if (fd < 0 || fd >= BB_INODE16_NUM) {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = BBFS_ERR_INVALID;

    fat = __osBbFat;
    in = &fat->inode[fd];

    if (in->type != 0) {
        sb->type = in->type;
        sb->size = in->size;

        if (blockList != NULL && listLen != 0) {
            u16 b = in->block;

            for (i = 0; b != 0xFFFF && i < listLen; i++) {
                blockList[i] = b;
                b = fat[b / 0x1000].entry[b % 0x1000];
            }

            if (i < listLen) {
                blockList[i] = 0;
            }
        }

        rv = 0;
    }

    __osBbFsRelAccess();
    return rv;
}
