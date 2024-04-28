#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFRename(const char* old, const char* new) {
    unsigned char fold[BB_INODE16_NAMELEN];
    unsigned char fnew[BB_INODE16_NAMELEN];
    s32 rv;
    s32 inew = -1;
    s32 iold = -1;
    BbFat16* fat;
    int i;

    __osBbFsFormatName(fold, old);
    __osBbFsFormatName(fnew, new);

    if (fold[0] == '\0' || fnew[0] == '\0') {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;

    for (i = 0; i < BB_INODE16_NUM; i++) {
        if (fat->inode[i].type != 0) {
            if (bcmp(fnew, fat->inode[i].name, BB_INODE16_NAMELEN * sizeof(unsigned char)) == 0) {
                inew = i;
            } else if (bcmp(fold, fat->inode[i].name, BB_INODE16_NAMELEN * sizeof(unsigned char)) == 0) {
                iold = i;
            }
        }
    }

    rv = BBFS_ERR_ENTRY;
    if (iold != -1) {
        if (inew != -1) {
            u16 b = fat->inode[inew].block;
            while (b != BBFS_BLOCK_EOC) {
                u16 next = BBFS_NEXT_BLOCK(fat, b);
                BBFS_NEXT_BLOCK(fat, b) = BBFS_BLOCK_FREE;
                b = next;
            }
            bzero(&fat->inode[inew], sizeof(BbInode));
        }

        bcopy(fnew, fat->inode[iold].name, BB_INODE16_NAMELEN * sizeof(unsigned char));
        rv = __osBbFsSync(FALSE);
    }

    __osBbFsRelAccess();
    return rv;
}
