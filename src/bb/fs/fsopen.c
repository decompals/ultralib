#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "PR/bbfs.h"

s32 osBbFOpen(const char* name, const char* mode) {
    unsigned char fname[BB_INODE16_NAMELEN];
    int i;
    s32 rv = BBFS_ERR_INVALID;
    BbFat16* fat;

    __osBbFsFormatName(fname, name);
    if (fname[0] == '\0') {
        return rv;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;

    // Locate the inode for this file by name
    for (i = 0; i < BB_INODE16_NUM; i++) {
        if (fat->inode[i].type != 0 && bcmp(fname, fat->inode[i].name, BB_INODE16_NAMELEN) == 0) {
            // If found, return the inode index as the file descriptor
            rv = i;
            goto found;
        }
    }
    // If not found, error
    rv = BBFS_ERR_ENTRY;

found:
    __osBbFsRelAccess();
    return rv;
}
