#include "PR/os_internal.h"
#include "PR/os_bbfs_export.h"
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
                while (b != 0xFFFF) {
                    u16 temp_v0; // not present on mdebug

                    temp_v0 = BBFS_NEXT_BLOCK(fat, b);
                    BBFS_NEXT_BLOCK(fat, b) = 0;
                    b = temp_v0;
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
