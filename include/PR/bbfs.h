#ifndef BBFS_H_
#define BBFS_H_

#include "ultratypes.h"

#define BB_FL_BLOCK_SIZE	16384
#define BB_INODE16_NAMELEN	11	/* maximum name length */
#define BB_INODE16_NUM      409

#define BBFS_ERR_NO_CARD    (-1)    /* card not present */
#define BBFS_ERR_FAIL       (-2)    /* operation failed */
#define BBFS_ERR_INVALID    (-3)    /* invalid parameters */
#define BBFS_ERR_CHANGED    (-4)    /* card changed */
#define BBFS_ERR_UNINIT     (-5)    /* fs uninitialized */
#define BBFS_ERR_EXISTS     (-6)    /* file exists */
#define BBFS_ERR_SPACE      (-7)    /* no space */
#define BBFS_ERR_ENTRY      (-8)    /* no entry */

/* Used for saving auxilliary game state data */
#define BBFS_ERR_STATE          (-9)   /* invalid state */
#define BBFS_ERR_STATE_LIMIT   (-10)   /* state limit reached */

typedef u16 BbFatEntry;

typedef struct {
    /* 0x0000 */ u8 name[BB_INODE16_NAMELEN];
    /* 0x000B */ u8 type;
    /* 0x000C */ u16 block;
    /* 0x000E */ u16 pad;
    /* 0x0010 */ u32 size;
} BbInode; // size = 0x14

typedef struct {
    /* 0x0000 */ BbFatEntry entry[4096];
    /* 0x2000 */ BbInode inode[BB_INODE16_NUM];
    /* 0x3FF4 */ u8 magic[4];
    /* 0x3FF8 */ u32 seq;
    /* 0x3FFC */ u16 link;
    /* 0x3FFE */ u16 cksum;
} BbFat16; // size = 0x4000

// `fat` is a `BbFat16` pointer
#define BBFS_NEXT_BLOCK(fat, b) (fat[b >> 0xC].entry[b & 0xFFF])

extern BbFat16* __osBbFat;

typedef struct {
    /* 0x0000 */ u8 root[32768];
} OSBbFs; // size = 0x8000

typedef struct {
    /* 0x0000 */ u16 files;
    /* 0x0002 */ u16 blocks;
    /* 0x0004 */ u16 freeFiles;
    /* 0x0006 */ u16 freeBlocks;
} OSBbStatFs; // size = 0x8

typedef struct {
    /* 0x0000 */ char name[13];
    /* 0x000D */ u8 type;
    /* 0x0010 */ u32 size;
} OSBbDirEnt; // size = 0x14

typedef struct {
    /* 0x0000 */ u8 type;
    /* 0x0004 */ u32 size;
} OSBbStatBuf; // size = 0x8

s32 osBbFOpen(const char* name, const char* mode);
s32 osBbFWrite(s32 fd, u32 off, void* buf, u32 len);
s32 osBbFRead(s32 fd, u32 off, void* buf, u32 len);
s32 osBbFClose(s32 fd);
s32 osBbFStatFs(OSBbStatFs* statfs);
s32 osBbFInit(OSBbFs* fs);
s32 osBbFDelete(const char* name);
s32 osBbFCreate(const char* name, u8 type, u32 len);
s32 osBbFRename(const char* old, const char* new);
s32 osBbFStat(s32 fd, OSBbStatBuf* sb, u16* blockList, u32 listLen);
s32 osBbFReadDir(OSBbDirEnt* dir, u32 count);
s32 osBbFRepairBlock(s32 fd, u32 off, void* buf, u32 len);
s32 osBbFShuffle(s32 sfd, s32 dfd, s32 release, void* buf, u32 len);
s32 osBbFAutoSync(u32 on);
s32 osBbFSync(void);

// private

s32 __osBbFsGetAccess(void);
void __osBbFsRelAccess(void);

void __osBbFsFormatName(char*, const char*);

void __osBbFCheck(void);

s32 __osBbFsSync(int force);

u16 __osBbFReallocBlock(BbInode* in, u16 block, BbFatEntry newVal);

extern u16 __osBbFatBlock;
extern u16 __osBbFsBlocks;
extern BbFat16* __osBbFat;
extern u8 __osBbFsAutoSync;

#endif
