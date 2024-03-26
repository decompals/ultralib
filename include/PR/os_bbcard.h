#ifndef _os_bb_card_h_
#define _os_bb_card_h_

#include <PR/ultratypes.h>

extern s32  osBbCardEraseBlock(u32 dev, u16 block);
extern s32  osBbCardEraseBlocks(u32 dev, const u16 block[], u32 n);
extern s32  osBbCardReadBlock(u32 dev, u16 block, void* addr, void* spare);
extern s32  osBbCardWriteBlock(u32 dev, u16 block, const void* addr,
	                       const void* spare);
extern s32  osBbCardWriteBlocks(u32 dev, const u16 block[], u32 n,
	                        const void* addr, const void* spare);

#endif
