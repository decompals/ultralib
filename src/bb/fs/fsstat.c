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

    // Access the inode for this file, skip and error if the inode is marked as free
    in = &fat->inode[fd];
    if (in->type != 0) {
        // Retrieve file type and size
        sb->type = in->type;
        sb->size = in->size;

        // If the caller requested a NAND block list, build one
        if (blockList != NULL && listLen != 0) {
            // Visit all blocks linked in the FAT until the list length or the blocks are exhausted
            u16 b = in->block;
            for (i = 0; b != BBFS_BLOCK_EOC && i < listLen; i++) {
                blockList[i] = b;
                b = BBFS_NEXT_BLOCK(fat, b);
            }

            // If the list was not filled to capacity, fill the remainder with 0s
            if (i < listLen) {
                blockList[i] = 0;
            }
        }

        rv = 0;
    }

    __osBbFsRelAccess();
    return rv;
}
