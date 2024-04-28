#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFDelete(const char* name) {
    unsigned char fname[BB_INODE16_NAMELEN];
    s32 rv;
    BbFat16* fat;
    int i;

    __osBbFsFormatName(fname, name);

    if (fname[0] == '\0') {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = BBFS_ERR_ENTRY;
    fat = __osBbFat;

    for (i = 0; i < BB_INODE16_NUM; i++) {
        u16 b;

        if (fat->inode[i].type != 0) {
            if (bcmp(fname, fat->inode[i].name, BB_INODE16_NAMELEN * sizeof(unsigned char)) == 0) {
                b = fat->inode[i].block;
                while (b != BBFS_BLOCK_EOC) {
                    u16 next = BBFS_NEXT_BLOCK(fat, b);
                    BBFS_NEXT_BLOCK(fat, b) = BBFS_BLOCK_FREE;
                    b = next;
                }

                bzero(&fat->inode[i], sizeof(fat->inode[i]));
                rv = __osBbFsSync(FALSE);
                break;
            }
        }
    }

    __osBbFsRelAccess();
    return rv;
}
