#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFReadDir(OSBbDirEnt* dir, u32 count) {
    s32 rv;
    s32 i;
    s32 j;
    BbFat16* fat;
    OSBbDirEnt* d;

    d = dir;
    rv = __osBbFsGetAccess();

    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;

    for (i = 0, j = 0; i < BB_INODE16_NUM; i++) {
        unsigned char* var_s2 = fat->inode[i].name;
        s32 k;

        if (fat->inode[i].type == 0) {
            continue;
        }

        rv++;
        if (d != NULL && j < count) {
            d->type = fat->inode[i].type;
            d->size = fat->inode[i].size;

            for (k = 0; var_s2[k] != '\0' && k < 8; k++) {
                d->name[k] = var_s2[k];
            }

            if (var_s2[8] != '\0') {
                d->name[k] = '.';
                bcopy(&var_s2[8], &d->name[k] + 1, 3);
                d->name[k+4] = '\0';
            } else {
                d->name[k] = '\0';
            }

            d++;
            j++;
        }
    }

    __osBbFsRelAccess();
    return rv;
}
