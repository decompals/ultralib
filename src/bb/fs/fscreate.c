#include "PR/os_internal.h"
#include "PR/bbcard.h"
#include "PR/bbfs.h"

void __osBbFsFormatName(char* fname, const char* name) {
    int i;
    int j;

    for (i = 0; name[i] != '\0' && name[i] != '.' && i < 8; i++) {
        fname[i] = name[i];
    }

    for (j = i; j < 8; j++) {
        fname[j] = '\0';
    }

    if (name[i] == '.') {
        i++;

        while (name[i] != '\0') {
            if (j >= BB_INODE16_NAMELEN) {
                return;
            }
            fname[j++] = name[i++];
        }
    }

    while (j < BB_INODE16_NAMELEN) {
        fname[j++] = '\0';
    }
}

s32 osBbFCreate(const char* name, u8 type, u32 len) {
    u16 i;
    u16 b;
    u16 prev = BBFS_BLOCK_FREE;
    BbInode* in = NULL;
    BbFat16* fat;
    s32 rv;
    s32 incr;
    char fname[BB_INODE16_NAMELEN];

    if (len % BB_FL_BLOCK_SIZE != 0) {
        return BBFS_ERR_INVALID;
    }

    __osBbFsFormatName(fname, name);
    if (fname[0] == '\0') {
        return BBFS_ERR_INVALID;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = BBFS_ERR_EXISTS;
    fat = __osBbFat;

    for (i = 0; i < BB_INODE16_NUM; i++) {
        if (fat->inode[i].type != 0 && bcmp(fname, fat->inode[i].name, BB_INODE16_NAMELEN) == 0) {
            // File already exists, error
            goto err;
        }
        if (fat->inode[i].type == 0 && in == NULL) {
            // Found a free inode
            in = &fat->inode[i];
        }
    }
    if (in == NULL) {
        // Couldn't find a free inode, no space for new file
        rv = BBFS_ERR_SPACE;
        goto err;
    }

    if (len > 64 * BB_FL_BLOCK_SIZE) {
        // Large files search low->high in FAT?
        b = BBFS_SKSA_LIMIT;
        incr = 1;
    } else {
        // Small files search high->low in FAT?
        incr = -1;
        b = __osBbFsBlocks - 1;
    }

    // Find free blocks to store the file in
    in->block = BBFS_BLOCK_EOC;

    for (i = 0; i < (len + BB_FL_BLOCK_SIZE - 1) / BB_FL_BLOCK_SIZE; i++) {
        while (b < __osBbFsBlocks && BBFS_NEXT_BLOCK(fat, b) != 0) {
            // Search until a free block is found
            b += incr;
        }
        if (b >= __osBbFsBlocks) {
            // Exhausted all blocks, no room
            goto not_enough_free;
        }

        // Mark block as occupied and end of the chain
        BBFS_NEXT_BLOCK(fat, b) = BBFS_BLOCK_EOC;

        if (prev != BBFS_BLOCK_FREE) {
            // Link prev to new block
            BBFS_NEXT_BLOCK(fat, prev) = b;
        } else {
            // No prev, link inode to first block
            in->block = b;
        }
        prev = b;
    }

    // Fill in inode
    bcopy(fname, in, BB_INODE16_NAMELEN);
    in->type = 1;
    in->size = len;

    if (__osBbFsSync(FALSE) == 0) { // Sync fs state to flash?
        // Return index of inode as file descriptor
        rv = in - fat->inode;
    } else {
not_enough_free:
        // Not enough free blocks to store the file, or sync to flash failed

        // Revert FAT changes
        b = in->block;
        while (b != BBFS_BLOCK_EOC) { // While not at the end of the chain
            u16 next = BBFS_NEXT_BLOCK(fat, b);
            BBFS_NEXT_BLOCK(fat, b) = BBFS_BLOCK_FREE; // mark the block as free
            b = next;
        }

        // Clear inode
        in->block = BBFS_BLOCK_FREE;
        in->size = 0;
        in->name[0] = '\0';

        // Return error
        rv = BBFS_ERR_SPACE;
    }

err:
    __osBbFsRelAccess();
    return rv;
}
