#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "PR/bbfs.h"

BbFat16* __osBbFat;
u16 __osBbFatBlock;
u16 __osBbFsBlocks;
static u8 __osBbFsAccessQueueInit;
static OSMesg accessBuf[1];
static OSMesgQueue __osBbFsAccessQueue;

u8 __osBbFsAutoSync = TRUE;

static void createAccessQueue(void) {
    __osBbFsAccessQueueInit = TRUE;
    osCreateMesgQueue(&__osBbFsAccessQueue, accessBuf, 1);
    osSendMesg(&__osBbFsAccessQueue, NULL, OS_MESG_NOBLOCK);
}

s32 __osBbFsGetAccess(void) {
    s32 rv = osBbCardUnhappy();
    if (rv != 0) {
        return rv;
    }
    if (__osBbFat == NULL) {
        return BBFS_ERR_UNINIT;
    }

    osRecvMesg(&__osBbFsAccessQueue, NULL, OS_MESG_BLOCK);
    return 0;
}

void __osBbFsRelAccess(void) {
    osSendMesg(&__osBbFsAccessQueue, NULL, OS_MESG_NOBLOCK);
}

/**
 * Compute a checksum for the FAT contents, rolling 16-bit sum
 */
static u16 csum(void* p) {
    u16* x = (u16*)p;
    u16 sum = 0;
    u16 i;

    for (i = 0; i < sizeof(BbFat16) / sizeof(*x); i++) {
        sum += x[i];
    }
    return sum;
}

void __osBbFCheck(void) {
    BbFat16* fat = __osBbFat;
    BbInode* in;
    u16 i;
    u16 j;
    static u32 map[256];
    BbFatEntry b;
    u32 size;

retry:
    bzero(map, sizeof(map));

    for (i = 0; i < BB_INODE16_NUM; i++) {
        in = &fat->inode[i];

        if (in->type == 0) {
            continue;
        }

        size = 0;

        for (b = in->block; b != BBFS_BLOCK_EOC; b = BBFS_NEXT_BLOCK(fat, b)) {
            if (b < 64) {
                goto delete_inode;
            }
            if (b >= __osBbFsBlocks - BBFS_FAT_LIMIT) {
                goto delete_inode;
            }

            if (map[b >> 5] & (1 << (b & 0x1F))) {
                goto delete_inode;
            }
            map[b >> 5] |= (1 << (b & 0x1F));

            size += BB_FL_BLOCK_SIZE;
        }

        if (size != in->size) {
            goto delete_inode;
        }
        if (in->name[0] == '\0') {
            goto delete_inode;
        }

        for (j = 1; j < BB_INODE16_NAMELEN; j++) {
            if (in->name[j] != '\0' && (in->name[j] < ' ' || in->name[j] > '~')) {
                goto delete_inode;
            }
        }
        continue;

delete_inode:
        bzero(in->name, BB_INODE16_NAMELEN);
        in->size = 0;
        in->type = 0;
        in->block = BBFS_BLOCK_FREE;
        goto retry;
    }

    for (b = BBFS_SKSA_LIMIT; b < __osBbFsBlocks - BBFS_FAT_LIMIT; b++) {
        if (map[b >> 5] & (1 << (b & 0x1F))) {
            continue;
        }

        if (BBFS_NEXT_BLOCK(fat, b) != BBFS_BLOCK_BAD) {
            BBFS_NEXT_BLOCK(fat, b) = BBFS_BLOCK_FREE;
        }
    }

    for (i = 0; i < BBFS_FAT_LIMIT; i++) {
        if (BBFS_NEXT_BLOCK(fat, __osBbFsBlocks - 1 - i) != BBFS_BLOCK_BAD) {
            BBFS_NEXT_BLOCK(fat, __osBbFsBlocks - 1 - i) = BBFS_BLOCK_RESERVED;
        }
    }

    // reserve SKSA
    for (i = 0; i < BBFS_SKSA_LIMIT; i++) {
        if (BBFS_NEXT_BLOCK(fat, i) != BBFS_BLOCK_BAD) {
            BBFS_NEXT_BLOCK(fat, i) = BBFS_BLOCK_RESERVED;
        }
    }
}

static s32 __osBbFsReadFat(u16 block, BbFat16* fat, const u8* magic) {
    s32 rv;

    // Read NAND block (0x4000 bytes)
    rv = osBbCardReadBlock(0, block, fat, NULL);
    if (rv == 0) {
        // Confirm checksum
        if (csum(fat) != BBFS_CHECKSUM_VALUE) {
            return 1;
        }
        // Confirm magic value
        if (bcmp(fat->magic, magic, sizeof(fat->magic)) != 0) {
            return 1;
        }
    }
    return rv;
}

static s32 __osBbFsReadFatLinks(BbFat16* fat) {
    s32 k;
    s32 rv;

    for (k = 1; k < __osBbFsBlocks >> 0xC; k++) {
        if (fat[k - 1].link == 0) {
            return 1;
        }
        rv = __osBbFsReadFat(fat[k - 1].link, &fat[k], "BBFL");
        if (rv != 0) {
            return rv;
        }
        if (fat[k - 1].seq != fat[k].seq) {
            return 1;
        }
    }
    return 0;
}

s32 osBbFInit(OSBbFs* fs) {
    u32 i;
    u32 start;
    s32 best = -1;
    s32 seq = 0;
    s32 rv;

    if (!__osBbFsAccessQueueInit) {
        createAccessQueue();
    }

    osRecvMesg(&__osBbFsAccessQueue, NULL, OS_MESG_BLOCK);
    osBbCardInit();

    __osBbFat = NULL;

    rv = osBbCardUnhappy();
    if (rv != 0) {
        goto err;
    }

    start = (__osBbFsBlocks = osBbCardBlocks(0)) - 1;

retry:
    for (i = 0; i < BBFS_FAT_LIMIT; i++) {
        rv = __osBbFsReadFat(start - i, (BbFat16*)fs, "BBFS");
        if (rv == 0) {
            rv = __osBbFsReadFatLinks((BbFat16*)fs);
            if (rv == 0) {
                if (((BbFat16*)fs)->seq >= seq) {
                    best = i;
                    seq = ((BbFat16*)fs)->seq;
                }
            }
        }
        if (rv < 0 && rv != BBFS_ERR_FAIL) {
            goto err;
        }
    }

    if (best == -1) {
        rv = BBFS_ERR_FAIL;
        goto err;
    }

    if (osBbCardReadBlock(0, start - best, fs, NULL) != 0) {
        goto retry;
    }
    if (csum(fs) != BBFS_CHECKSUM_VALUE) {
        goto retry;
    }
    if (seq != ((BbFat16*)fs)->seq) {
        goto retry;
    }
    if (__osBbFsReadFatLinks((BbFat16*)fs) != 0) {
        goto retry;
    }

    __osBbFat = (BbFat16*)fs;
    __osBbFatBlock = best;
    __osBbFsAutoSync = TRUE;
    rv = 0;
    __osBbFCheck();

err:
    __osBbFsRelAccess();
    return rv;
}

s32 osBbFStatFs(OSBbStatFs* statfs) {
    u16 b;
    u16 i;
    u16 j;
    s32 rv;
    BbFat16* fat;

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;

    // Count the number of occupied files
    j = 0;
    for (i = 0; i < BB_INODE16_NUM; i++) {
        if (fat->inode[i].type != 0) {
            j++;
        }
    }

    statfs->files = j;
    statfs->freeFiles = BB_INODE16_NUM - j;

    i = j = 0;
    for (b = BBFS_SKSA_LIMIT; b < __osBbFsBlocks - BBFS_FAT_LIMIT; b++) {
        BbFatEntry next = BBFS_NEXT_BLOCK(fat, b);

        if (next == BBFS_BLOCK_FREE) {
            // Count number of free blocks
            j++;
        } else if (next != BBFS_BLOCK_RESERVED && next != BBFS_BLOCK_BAD) {
            // Count number of occupied blocks that are not bad or reserved
            i++;
        }
    }

    statfs->blocks = i;
    statfs->freeBlocks = j;

    __osBbFsRelAccess();
    return 0;
}
