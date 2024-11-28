#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "PR/bbfs.h"

s32 __osBbFsSync(int force) {
    BbFat16* fat = __osBbFat;
    u32 target = __osBbFatBlock;
    u16 sum;
    u16* p;
    u16 link = 0;
    int tries;
    int k;

    if (!force && !__osBbFsAutoSync) {
        return 0;
    }

    k = __osBbFsBlocks >> 0xC;
    while (--k >= 0) {
        fat[k].seq++;
        fat[k].link = link;
        fat[k].cksum = 0;

        // Compute new checksum
        sum = 0;
        for (p = &fat[k].entry[0]; p < &fat[k].entry[8192]; p++) { // wtf?
            sum += *p;
        }
        fat[k].cksum = BBFS_CHECKSUM_VALUE - sum;

        // Write the FAT to the card
        for (tries = 0; tries < 16; tries++) {
            target++;
            if (target >= BBFS_FAT_LIMIT) {
                target = 0;
            }

            link = __osBbFsBlocks - target - 1;
            if (osBbCardEraseBlock(0, link) == 0 &&
                osBbCardWriteBlock(0, __osBbFsBlocks - target - 1, &fat[k], NULL) == 0) {
                goto success;
            }
            // retry
        }
        // ran out of retries
        return BBFS_ERR_FAIL;
success:;
    }
    __osBbFatBlock = target;
    return 0;
}

s32 osBbFAutoSync(u32 on) {
    __osBbFsAutoSync = on;
    return 0;
}

s32 osBbFSync(void) {
    s32 rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = __osBbFsSync(TRUE);

    __osBbFsRelAccess();
    return rv;
}
