#ifndef _BBCARD_H_
#define _BBCARD_H_

#include "ultratypes.h"
#include "bbnand.h"

#define BBCARD_ERR_NO_CARD	(-1)	/* no card present */
#define BBCARD_ERR_FAIL		(-2)	/* operation failed */
#define BBCARD_ERR_INVALID	(-3)	/* invalid parameters */
#define BBCARD_ERR_CHANGED	(-4)	/* card changed */

void osBbCardInit(void);
s32 osBbCardReadBlock(u32 dev, u16 block, void* addr, void* spare);
s32 osBbCardEraseBlock(u32 dev, u16 block);
s32 osBbCardWriteBlock(u32 dev, u16 block, void* addr, void* spare);
s32 osBbCardStatus(u32 dev, u8* status);
s32 osBbCardChange(void);
s32 osBbCardClearChange(void);
u32 osBbCardBlocks(u32 dev);
s32 osBbCardUnhappy(void);

/* private */

s32 __osBbCardGetAccess(void);
void __osBbCardRelAccess(void);
s32 __osBbCardWaitEvent(void);
s32 __osBbCardFlushEvent(void);
s32 __osBbCardPresent(void);

extern u16 __osBbCardBlocks;
extern u8 __osBbCardChange;
extern u8 __osBbCardInit;
extern u8 __osBbCardMultiplane;

#endif
