#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFClose(s32 fd) {
    BbFat16* fat;
    s32 rv;

    if (fd < 0 || fd >= BB_INODE16_NUM) {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv != 0) {
        return rv;
    }

    fat = __osBbFat;
    if (fat->inode[fd].type == 0) {
        rv = BBFS_ERR_INVALID;
    }
    __osBbFsRelAccess();
    return rv;
}
