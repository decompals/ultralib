#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFDelete(const char* name) {
    unsigned char fname[BB_INODE16_NAMELEN];
    s32 rv;
    BbFat16* fat;
    int i;

    __osBbFsFormatName(fname, name);

    if (fname[0] == 0) {
        return -3;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = -8;

    fat = __osBbFat;
    for (i = 0; i < BB_INODE16_NUM; i++) {
        u16 b;

        if (fat->inode[i].type != 0) {
            if (bcmp(fname, fat->inode[i].name, BB_INODE16_NAMELEN) == 0) {
                b = fat->inode[i].block;
                while (b != 0xFFFF) {
                    u16 temp_v0; // not present on mdebug

                    temp_v0 = fat[b >> 0xC].entry[b & 0xFFF];
                    fat[b >> 0xC].entry[b & 0xFFF] = 0;
                    b = temp_v0;
                }

                bzero(&fat->inode[i], sizeof(fat->inode[i]));
                rv = __osBbFsSync(0);
                break;
            }
        }
    }

    __osBbFsRelAccess();
    return rv;
}
