#include "PR/os_internal.h"
#include "PR/os_pi.h"
#include "PR/bcp.h"
#include "PR/bbcard.h"
#include "macros.h"

#include "../usb/usb.h"

typedef struct /* size=0x8 */ {
    /* 0x0000 */ u8 type;
    /* 0x0004 */ u32 size;
} OSBbStatBuf;

typedef struct /* size=0x2C */ {
    /* 0x0000 */ u32 eepromAddress;
    /* 0x0004 */ u32 eepromSize;
    /* 0x0008 */ u32 flashAddress;
    /* 0x000C */ u32 flashSize;
    /* 0x0010 */ u32 sramAddress;
    /* 0x0014 */ u32 sramSize;
    /* 0x0018 */ u32 pakAddress[4];
    /* 0x0028 */ u32 pakSize;
} OSBbStateVector;

typedef struct /* size=0x44 */ {
    /* 0x0000 */ OSBbStateVector state;
    /* 0x002C */ u32 romBase;
    /* 0x0030 */ s32 tvType;
    /* 0x0034 */ u32 memSize;
    /* 0x0038 */ u32 errataSize;
    /* 0x003C */ u32 errataAddress;
    /* 0x0040 */ u32 magic;
} OSBbLaunchMetaData;

extern u32 __osBbIsBb;

u32 osBbLoadApp(OSBbLaunchMetaData* md, u16* blockList, s32 listSize, s32 loadAll) {
    u32 addr;
    OSMesgQueue dmaMessageQ;
    OSMesg dmaMessageBuf;
    OSIoMesg dmaMesg;
    OSPiHandle* handler;
    s32 fileSize = listSize << 14;

    osCreateMesgQueue(&dmaMessageQ, &dmaMessageBuf, 1);

    if (listSize >= 0) {
        // 0-terminate the block list
        blockList[listSize] = 0;

        // map the NAND fs blocks into cart space
        if (osBbAtbSetup(K1_TO_PHYS(md->romBase), blockList, listSize + 1) < 0) {
            return 0;
        }
    }

    handler = osCartRomInit();

    IO_WRITE(PI_44_REG, 0);
    IO_WRITE(PI_48_REG, NAND_ATB_CONFIG(0, TRUE));

    // Read app entry point from ROM header
    osEPiReadIo(handler, 8, &addr);

    dmaMesg.hdr.pri = OS_MESG_PRI_NORMAL;
    dmaMesg.hdr.retQueue = &dmaMessageQ;

    if (loadAll) {
        // load ROM header, IPL and app
        dmaMesg.dramAddr = addr - 0x1000;
        dmaMesg.devAddr = 0;
        dmaMesg.size = fileSize;
    } else {
        // Only load app (1st MB)
        dmaMesg.dramAddr = addr;
        dmaMesg.devAddr = 0x1000;
        // Ensure the DMA will not read out-of-bounds of the target file
        dmaMesg.size = MIN(fileSize - 0x1000, 0x100000);
    }

    // DMA requested data and wait for completion
    osEPiStartDma(handler, &dmaMesg, 0);
    osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);

    // DMA extra "errata" data?
    if (md->errataSize != 0 && md->errataSize < fileSize) {
        dmaMesg.hdr.pri = OS_MESG_PRI_NORMAL;
        dmaMesg.hdr.retQueue = &dmaMessageQ;
        dmaMesg.dramAddr = md->errataAddress;
        dmaMesg.devAddr = fileSize - md->errataSize;
        dmaMesg.size = md->errataSize;
        osEPiStartDma(handler, &dmaMesg, 0);
        osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
    }

    // Write back the data cache and invalidate the instruction cache
    osWritebackDCacheAll();
    osInvalICache((void*)K0BASE, 0x00010000);

    // Set up libultra boot parameters
    osRomBase = md->romBase;
    osMemSize = md->memSize;
    osTvType = md->tvType;

    osBbAuxDataSetLimit(md->magic & 0xFF);
    return addr;
}

s32 osBbExecApp(u32 addr) {
    int (*f)() = (void*)addr;

    // Set all PI operations allowed
    IO_WRITE(PI_54_REG, 0xFF);

    // USB?
    IO_WRITE(USB_REG_40010(0), 1);
    IO_WRITE(USB_REG_40010(1), 1);

    if (__osBbIsBb >= 2) {
        IO_WRITE(MI_3C_REG, 0x01000000);
        IO_WRITE(MI_3C_REG, 0x02000000);
        IO_WRITE(MI_14_REG, 0x03000000);
    } else {
        IO_WRITE(MI_14_REG, 0x01000000);
    }

    __osDisableInt();
    return f();
}

/**
 * @param fd        File descriptor 
 * @param name      File name
 * @param md        Content Meta Data (CMD)
 * @param buffer    Shared buffer, either contains the FS block list as an array of u16 for .aes files
 *                  or it's a 0x4000 byte buffer used to load the first block of ROM data into
 * @return s32 
 */
s32 osBbExecFile(s32 fd, char* name, OSBbLaunchMetaData* md, u8* buffer) {
    u32 addr;
    int (*f)();
    s32 rv = 0;
    s32 i;
    s32 off;

    i = strlen(name);

    if (i >= 4 && name[i - 4] == '.' && name[i - 3] == 'a' && name[i - 2] == 'e' && name[i - 1] == 's') {
        // This procedure maps fs blocks into cart space and loads only the first megabyte into RAM
        s32 listSize;
        OSBbStatBuf sb;
        OSMesgQueue dmaMessageQ;
        OSMesg dmaMessageBuf;
        OSIoMesg dmaMesg;
        OSPiHandle* handler;

        osCreateMesgQueue(&dmaMessageQ, &dmaMessageBuf, 1);

        // obtain the size of all blocks
        rv = osBbFStat(fd, &sb, (u16*)buffer, 0x1000);
        if (rv < 0) {
            return rv;
        }

        listSize = sb.size / 0x4000;

        if (listSize >= 0) {
            // 0-terminate the block list
            ((u16*)buffer)[listSize] = 0;

            // map the NAND fs blocks into cart space
            rv = osBbAtbSetup(md->romBase, (u16*)buffer, listSize + 1);
            if (rv < 0) {
                return rv;
            }
        }

        handler = osCartRomInit();

        IO_WRITE(PI_50_REG, 0);
        IO_WRITE(PI_44_REG, 0);
        IO_WRITE(PI_48_REG, NAND_ATB_CONFIG(0, TRUE));

        // Read game entry point from ROM header
        osEPiReadIo(handler, 8, &addr);
        f = (void*)addr;

        // DMA 1st MB (or less if the file is smaller), skipping ROM header and IPL
        dmaMesg.hdr.pri = OS_MESG_PRI_NORMAL;
        dmaMesg.hdr.retQueue = &dmaMessageQ;
        dmaMesg.dramAddr = (void*)addr;
        dmaMesg.devAddr = 0x1000;
        dmaMesg.size = MIN(sb.size - 0x1000, 0x100000);

        // DMA requested data and wait for completion
        osEPiStartDma(handler, &dmaMesg, OS_READ);
        osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);

        // DMA extra "errata" data?
        if (md->errataSize != 0 && md->errataSize < sb.size) {
            dmaMesg.hdr.pri = OS_MESG_PRI_NORMAL;
            dmaMesg.hdr.retQueue = &dmaMessageQ;
            dmaMesg.dramAddr = md->errataAddress;
            dmaMesg.devAddr = sb.size - md->errataSize;
            dmaMesg.size = md->errataSize;
            osEPiStartDma(handler, &dmaMesg, OS_READ);
            osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
        }
    } else {
        // This procedure will load the entire file into RAM at the entrypoint location without mapping any
        // fs blocks into cart space
        OSBbStatBuf sb;

        rv = osBbFStat(fd, &sb, NULL, 0);
        if (rv < 0) {
            return rv;
        }

        // Read first block, includes ROM header and IPL
        rv = osBbFRead(fd, 0, buffer, 0x4000);

        // Read entrypoint from ROM header
        addr = *(u32*)(buffer + 8);
        f = (void*)addr;

        // Skip ROM header and IPL, copy the rest to the entrypoint location
        bcopy(buffer + 0x1000, addr, 0x4000 - 0x1000);
        off = 0x4000;
        addr += 0x4000 - 0x1000;

        // Read block-by-block until error or reached EOF
        while (rv >= 0 && off < sb.size) {
            rv = osBbFRead(fd, off, addr, 0x4000);
            if (rv <= 0) {
                break;
            }
            off += 0x4000;
            addr += 0x4000;
        }
        if (rv < 0) {
            return rv;
        }
    }

    // Set up libultra boot parameters
    osRomBase = md->romBase;
    osMemSize = md->memSize;
    osTvType = md->tvType;

    // Write back the data cache and invalidate the instruction cache
    osWritebackDCacheAll();
    osInvalICache((void*)K0BASE, 0x10000);

    // Set all PI operations allowed
    IO_WRITE(PI_54_REG, 0xFF);
    IO_WRITE(PI_44_REG, 0x80000000);

    // USB?
    IO_WRITE(USB_REG_40010(0), 1);
    IO_WRITE(USB_REG_40010(1), 1);

    IO_WRITE(MI_14_REG, (__osBbIsBb >= 2) ? 0x03000000 : 0x01000000);

    __osDisableInt();
    return f();
}
