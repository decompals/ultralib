#include "PR/os_internal.h"

typedef struct /* size=0x8 */ {
    /* 0x0000 */ u8 type;
    /* 0x0004 */ u32 size;
} OSBbStatBuf;

typedef u16 BbFatEntry;

typedef struct /* size=0x14 */ {
    /* 0x0000 */ u8 name[11];
    /* 0x000B */ u8 type;
    /* 0x000C */ u16 block;
    /* 0x000E */ u16 pad;
    /* 0x0010 */ u32 size;
} BbInode;

typedef struct /* size=0x4000 */ {
    /* 0x0000 */ BbFatEntry entry[4096];
    /* 0x2000 */ BbInode inode[409];
    /* 0x3FF4 */ u8 magic[4];
    /* 0x3FF8 */ u32 seq;
    /* 0x3FFC */ u16 link;
    /* 0x3FFE */ u16 cksum;
} BbFat16;

extern BbFat16* __osBbFat;

s32 osBbFStat(s32 fd, OSBbStatBuf* sb, u16* blockList, u32 listLen) {
    int i;
    s32 rv;
    BbInode* in;
    BbFat16* fat;

    sizeof(BbFat16);

    if (fd >= 0x199U) {
        return -3;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    rv = -3;

    fat = __osBbFat;
    in = &fat->inode[fd];

    if (in->type != 0) {
        sb->type = in->type;
        sb->size = in->size;

        if (blockList != NULL && listLen != 0) {
            u16 b = in->block;
            i = 0;

            for (i = 0; b != 0xFFFF && i < listLen; i++) {
                blockList[i] = b;
                b = fat[b >> 0xC].entry[(b & 0xFFF)];
            }

            if (i < listLen) {
                blockList[i] = 0;
            }
        }
        rv = 0;
    }

    __osBbFsRelAccess();
    return rv;
}
