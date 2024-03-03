#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFClose(s32 fd) {
    BbFat16* fat;
    s32 rv;

    if (fd >= (u32)BB_INODE16_NUM) {
        return -3;
    }

    rv = __osBbFsGetAccess();
    if (rv != 0) {
        return rv;
    }

    fat = __osBbFat;
    if (fat->inode[fd].type == 0) {
        rv = -3;
    }
    __osBbFsRelAccess();
    return rv;
}
