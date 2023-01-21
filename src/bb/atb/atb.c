#include "PR/os_internal.h"
#include "PR/bcp.h"

s32 osBbAtbGetVAddr(void) {
    // skips first entry?
    u32 addr = (u16)IO_READ(PI_10500_REG(1)) * 0x4000;

    return addr;
}

/**
 * Maps filesystem blocks into cartridge memory space
 * 
 * @param vAddrBase     Base address to begin mapping at, should be a region that the PI can access
 * @param fsBlockList   filesystem block physical addresses to map, list should be 0-terminated
 * @param maxListLen    Size of the fsblockList
 * @return s32          0=success, -1=failure
 */
s32 osBbAtbSetup(u32 vAddrBase, u16* fsBlockList, u32 maxListLen) {
    int atbIndx;
    int maxPow2;
    int lenRun;
    int numBlks;
    int indx;
    int entryBlksPow2;
    int runStart = 0;
    u32 vAddr;
    u32 atbLower;

    // If the base vaddr is not aligned to 0x4000, fail
    if ((vAddrBase & ~0x3FFF) == 0) {
        return -1;
    }

    // ?
    IO_WRITE(PI_40_REG, 0x130);
    // associate fs block 0 with the region just before vAddrBase?
    IO_WRITE(PI_10500_REG(0), (0 << 16) | ((vAddrBase >> 14) - 1));

    atbIndx = 1;
    vAddr = vAddrBase;

    // Determine the number of blocks to map, the fsBlockList should be 0-terminated
    for (numBlks = 0; fsBlockList[numBlks] != 0; numBlks++) {
        if (numBlks == maxListLen - 1) {
            // Last block is not the 0-terminator, fail
            return -1;
        }
    }

    do {
        // Determine the largest power of 2 that fits in vAddr?
        for (maxPow2 = 0; (vAddr >> (maxPow2 + 14)) % 2 == 0; maxPow2++)
            ;

        // Get the largest power of 2 for contiguous blocks?
        for(indx = runStart, lenRun = 1, entryBlksPow2 = 0; fsBlockList[indx] + 1 == fsBlockList[indx + 1] && entryBlksPow2 < maxPow2; indx++) {
            lenRun++;
            if(!(lenRun & ((1 << (entryBlksPow2 + 1)) - 1))) {
                entryBlksPow2++;
            }
        }

        // ?
        IO_WRITE(PI_40_REG, 0x30 | entryBlksPow2);
        // Associate fsBlockList[runStart] with vAddr
        atbLower = (fsBlockList[runStart] << 16) | (vAddr >> 14);
        IO_WRITE(PI_10500_REG(atbIndx), atbLower);

        atbIndx++;
        vAddr += (16 << entryBlksPow2) * 0x400; // same as (1 << entryBlksPow2) * 0x4000, doesn't match though
        // skip 2^{entryBlksPow2} blocks
        runStart += 1 << entryBlksPow2;
    } while (runStart < numBlks);

    // Fill the remaining atb entries

    IO_WRITE(PI_40_REG, 0x30 | 0);

    atbLower = (fsBlockList[0] << 16) | (vAddr >> 14);
    while (atbIndx < 192) { // maximum number of entries?
        IO_WRITE(PI_10500_REG(atbIndx), atbLower);
        atbIndx++;
    }
    return 0;
}
