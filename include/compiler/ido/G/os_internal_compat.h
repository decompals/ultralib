#ifndef _IDO_G_OS_INTERNAL_COMPAT_H
#define _IDO_G_OS_INTERNAL_COMPAT_H

#include "G/os.h"
#include "G/os_internal.h"

#ifndef PFS_INODE_DIST_MAP
#define PFS_INODE_DIST_MAP 256
#define PFS_INODE_SIZE_PER_PAGE 128
#define DEF_DIR_PAGES 2
#define PFS_ID_0AREA 1
#define PFS_ID_1AREA 3
#define PFS_ID_2AREA 4
#define PFS_ID_3AREA 6
#define PFS_LABEL_AREA 7
#endif
#ifndef PFS_EOF
#define PFS_EOF 1
#define PFS_PAGE_NOT_USED 3
#define PFS_BANK_LAPPED_BY 8
#define PFS_SECTOR_PER_BANK 32
#define PFS_SECTOR_SIZE (PFS_INODE_SIZE_PER_PAGE / PFS_SECTOR_PER_BANK)
#endif
#ifndef EEPROM_TYPE_4K
#define EEPROM_TYPE_4K 0x01
#define EEPROM_TYPE_16K 0x02
#endif
#ifndef PI_DOMAIN1
#define PI_DOMAIN1 0
#define PI_DOMAIN2 1
#endif
#ifndef OS_IM_RCP
#define OS_IM_RCP 0x00000401
#endif

/*
 * Compatibility declarations for source files retained by the shared tree
 * but absent from the 2.0G archive. This is not an original SDK header.
 */
#ifndef OS_GBPAK_POWER
#define PFS_ERR_NO_GBCART 12
#define PFS_ERR_NEW_GBCART 13
#define PFS_GBPAK_INITIALIZED 0x10
#define OS_GBPAK_POWER 0x01
#define OS_GBPAK_RSTB_DETECTION 0x04
#define OS_GBPAK_RSTB_STATUS 0x08
#define OS_GBPAK_GBCART_PULL 0x40
#define OS_GBPAK_GBCART_ON 0x80
#define OS_GBPAK_POWER_OFF 0x00
#define OS_GBPAK_POWER_ON 0x01
#define OS_GBPAK_ROM_ID_SIZE 0x50

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
typedef struct {
    u16 fixed1;
    u16 start_address;
    u8 nintendo_chr[0x30];
    u8 game_title[16];
    u16 company_code;
    u8 body_code;
    u8 cart_type;
    u8 rom_size;
    u8 ram_size;
    u8 country_code;
    u8 fixed2;
    u8 version;
    u8 isum;
    u16 sum;
} OSGbpakId;

extern s32 osGbpakInit(OSMesgQueue*, OSPfs*, int);
extern s32 osGbpakPower(OSPfs*, s32);
extern s32 osGbpakGetStatus(OSPfs*, u8*);
extern s32 osGbpakReadWrite(OSPfs*, u16, u16, u8*, u16);
extern s32 osGbpakReadId(OSPfs*, OSGbpakId*, u8*);
extern s32 osGbpakCheckConnector(OSPfs*, u8*);
#endif
#endif

/* Preserve the private 2.0G OS header's external symbol ordering. */
#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
extern void (*__osErrorHandler)(s16, s16, ...);
typedef struct {
    u32 inst1;
    u32 inst2;
    u32 inst3;
    u32 inst4;
} __osExceptionVector;
extern __osExceptionVector __osExceptionPreamble[];
extern s32 (*__osHwIntTable[])(void);

typedef struct __OSEventState {
    OSMesgQueue* messageQueue;
    OSMesg message;
} __OSEventState;

extern __OSEventState __osEventStateTab[];
extern struct __osThreadTail {
    OSThread* next;
    OSPri priority;
} __osThreadTail;
extern OSThread* __osRunningThread;
extern OSThread* __osRunQueue;
extern OSThread* __osActiveQueue;
extern OSThread* __osFaultedThread;
extern OSTime __osCurrentTime;
extern u32 __osBaseCounter;
extern u32 __osViIntrCount;
extern u32 __osTimerCounter;
extern OSTimer* __osTimerList;
extern OSProf* __osProfileList;
extern OSProf* __osProfileListEnd;
extern u32 __osProfileOverflowBin;
extern OSTimer __osProfTimer;
extern OSMesgQueue __osProfTimerQ;
#endif

#endif
