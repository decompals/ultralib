#ifndef _BCP_H_
#define _BCP_H_
#ifdef BBPLAYER

#include "rcp.h"

/**
 * MIPS Interface (MI) Additional Registers
 */

//! MI_SK_EXCEPTION_REG ?
#define MI_14_REG (MI_BASE_REG + 0x14)



//! ?
#define MI_30_REG (MI_BASE_REG + 0x30)



//! ?
#define MI_38_REG (MI_BASE_REG + 0x38)


//! MI_HW_INTR_MASK_REG ?
#define MI_3C_REG (MI_BASE_REG + 0x3C)



/**
 * Peripheral Interface (PI) Additional Registers 
 */

//! PI_ATB_UPPER_REG ?
#define PI_40_REG (PI_BASE_REG + 0x40)



//! ?
#define PI_44_REG (PI_BASE_REG + 0x44)



/**
 *  [31]    : Exec
 *  [30]    : Raise interrupt when command is done
 *  [29:24] : ??
 *  [23:16] : NAND command
 *  [15]    : ??
 *  [14]    : Buffer Select
 *  [13:12] : Device Select
 *  [11]    : ECC
 *  [10]    : NAND command is multi-cycle
 *  [ 9: 0] : Transfer Length
 */
#define PI_48_REG (PI_BASE_REG + 0x48)



//! ?
#define PI_4C_REG (PI_BASE_REG + 0x4C)



//! PI_AES_CNT_REG ?
#define PI_50_REG (PI_BASE_REG + 0x50)



//! PI_ALLOWED_IO_REG ?
#define PI_54_REG (PI_BASE_REG + 0x54)



//! ?
#define PI_58_REG (PI_BASE_REG + 0x58)



//! ?
#define PI_5C_REG (PI_BASE_REG + 0x5C)

/**
 * [31:16] Box ID
 *   [31:30] ?? (osInitialize checks this and sets __osBbIsBb to 2 if != 0)
 *   [29:27] ?? (unused so far)
 *   [26:25] ?? (system clock speed identifier?)
 *   [24:22] ?? (bootrom, checked against MI_10_REG and copied there if mismatch)
 *   [21:16] ?? (unused so far)
 * [ 7: 4] GPIO direction control
 *     [7] RTC Data output enable
 *     [6] RTC Clock output enable
 *     [5] Error LED output enable
 *     [4] Power Control output enable
 * [ 3: 0] GPIO in/out value
 *     [3] RTC Data output value (0=low, 1=high)
 *     [2] RTC Clock output value (0=low, 1=high)
 *     [1] Error LED (0=on, 1=off)
 *     [0] Power Control (0=off, 1=on)
 */
#define PI_GPIO_REG (PI_BASE_REG + 0x60)

/* Box ID */
#define PI_GPIO_GET_BOXID(reg)  ((reg) >> 16)
#define PI_GPIO_BOXID_MASK_30_31    (3 << 30)

/* Input/Output enables */
#define PI_GPIO_I_PWR       ((0 << 0) << 4)
#define PI_GPIO_O_PWR       ((1 << 0) << 4)
#define PI_GPIO_I_LED       ((0 << 1) << 4)
#define PI_GPIO_O_LED       ((1 << 1) << 4)
#define PI_GPIO_I_RTC_CLK   ((0 << 2) << 4)
#define PI_GPIO_O_RTC_CLK   ((1 << 2) << 4)
#define PI_GPIO_I_RTC_DAT   ((0 << 3) << 4)
#define PI_GPIO_O_RTC_DAT   ((1 << 3) << 4)

/* Output controls */
/* Power */
#define PI_GPIO_PWR_OFF     (0 << 0)
#define PI_GPIO_PWR_ON      (1 << 0)
/* LED */
#define PI_GPIO_LED_ON      (0 << 1)
#define PI_GPIO_LED_OFF     (1 << 1)
/* RTC */
#define PI_GPIO_RTC_CLK_LO  (0 << 2)
#define PI_GPIO_RTC_CLK_HI  (1 << 2)
#define PI_GPIO_RTC_DAT_LO  (0 << 3)
#define PI_GPIO_RTC_DAT_HI  (1 << 3)

/* Input getters */
#define PI_GPIO_GET_PWR(reg)        (((reg) >> 0) & 1)
#define PI_GPIO_GET_LED(reg)        (((reg) >> 1) & 1)
#define PI_GPIO_GET_RTC_CLK(reg)    (((reg) >> 2) & 1)
#define PI_GPIO_GET_RTC_DAT(reg)    (((reg) >> 3) & 1)



//! ?
#define PI_64_REG (PI_BASE_REG + 0x64)



//! PI_CARD_BLK_OFFSET_REG ?
#define PI_70_REG (PI_BASE_REG + 0x70)



//! PI_EX_DMA_BUF ?
#define PI_10000_BUF(offset)                    (PI_BASE_REG + 0x10000 + (offset))
#define PI_NAND_DATA_BUFFER(bufSelect, offset)  PI_10000_BUF((bufSelect) * 0x200 + (offset))
#define PI_NAND_SPARE_BUFFER(bufSelect, offset) PI_10000_BUF(0x400 + (bufSelect) * 0x10 + (offset))


//! PI_ATB_LOWER_REG ?
#define PI_10500_REG(i) (PI_BASE_REG + 0x10500 + (i) * 4)



#define	RDB_WRITE16(addr,data) (*(vu16*)PHYS_TO_K1(addr)=(u16)(data))
#define	RDB_READ16(addr)       (*(vu16*)PHYS_TO_K1(addr))

#define RDB_20000_REG (PI_BASE_REG + 0x20000)
#define RDB_E0400_REG (PI_BASE_REG + 0xE0400)
#define RDB_E8000_REG (PI_BASE_REG + 0xE8000)

/**
 * Serial Interface (SI) Additional Registers
 */

//! ?
#define SI_0C_REG (SI_BASE_REG + 0x0C)



//! ?
#define SI_1C_REG (SI_BASE_REG + 0x1C)



#endif
#endif
