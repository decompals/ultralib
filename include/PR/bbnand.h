#ifndef _BBNAND_H_
#define _BBNAND_H_

#define NAND_BYTES_PER_PAGE     0x200
#define NAND_PAGE_SPARE_SIZE    0x10
#define NAND_PAGES_PER_BLOCK    32
#define NAND_BYTES_PER_BLOCK    (NAND_PAGES_PER_BLOCK * NAND_BYTES_PER_PAGE)

#define NAND_BLOCK_TO_PAGE(blockNum) ((blockNum) * NAND_PAGES_PER_BLOCK)
#define NAND_PAGE_TO_ADDR(pageNum) ((pageNum) * NAND_BYTES_PER_PAGE)
#define NAND_BLOCK_TO_ADDR(pageNum) ((pageNum) * NAND_BYTES_PER_BLOCK)

/* NAND Control (Read) */
#define NAND_STATUS_BUSY        (1 << 31)
#define NAND_STATUS_ERROR_DB    (1 << 10) /* Double-bit error, uncorrectable */
#define NAND_STATUS_ERROR_SB    (1 << 11) /* Single-bit error, automatically corrected by ECC if enabled */

/* NAND Control (Write) */
/* Write 0 to PI_48_REG to clear flash interrupt */
#define NAND_CTRL_CLR_INTR      0
/* Other write bits */
#define NAND_CTRL_EXEC         (1 << 31)     /* Execute NAND command immediately after write */
#define NAND_CTRL_INTR         (1 << 30)     /* Interrupt when done */
#define NAND_CTRL_UNK29        (1 << 29)
#define NAND_CTRL_UNK28        (1 << 28)
#define NAND_CTRL_UNK27        (1 << 27)
#define NAND_CTRL_UNK26        (1 << 26)
#define NAND_CTRL_UNK25        (1 << 25)
#define NAND_CTRL_UNK24        (1 << 24)
#define NAND_CTRL_CMD(cmd)     ((cmd) << 16) /* NAND command, sent directly to the NAND chip */
#define NAND_CTRL_UNK15        (1 << 15)
#define NAND_CTRL_BUFSEL(n)    ((n) << 14)   /* PI Buffer select, offset 0x000 or 0x200 */
#define NAND_CTRL_DEVICE(n)    ((n) << 12)   /* NAND device select (2-bit) */
#define NAND_CTRL_ECC          (1 << 11)     /* 1 if do ECC */
#define NAND_CTRL_MULTICYCLE   (1 << 10)     /* 1 if NAND command is multicycle */
#define NAND_CTRL_XFER_LEN(n)  ((n) << 0)    /* Transfer length in bytes between NAND and PI Buffer (10-bit) */

/* NAND Command IDs */
#define NAND_CMD_READ_0         0x00
#define NAND_CMD_READ_1         0x01
#define NAND_CMD_READ_2         0x50
#define NAND_CMD_READ_ID        0x90
#define NAND_CMD_RESET          0xFF
#define NAND_CMD_FILL_PAGE      0x80
#define NAND_CMD_WRITE_PAGE     0x10
#define NAND_CMD_DUMMY_PAGE     0x11
#define NAND_CMD_DUMMY_READ     0x03
#define NAND_CMD_COPY_BACK      0x8A
#define NAND_CMD_ERASE_PAGE     0x60
#define NAND_CMD_EXEC_ERASE     0xD0
#define NAND_CMD_READ_STATUS    0x70
#define NAND_CMD_READ_STATUS_MP 0x71

/* NAND Read Status (RSTAT) */

#define NAND_RSTAT_ERASE_OK 0xC0

/* NAND Read ID Response */

#define NAND_ID_MULTIPLANE 0xC0

#define NAND_ID_GET_MFG(status)        (((status) >> 0x18) & 0xFF)
#define NAND_ID_GET_TYPE(status)       (((status) >> 0x10) & 0xFF)
#define NAND_ID_IS_MULTIPLANE(status) ((((status) >> 0x00) & 0xFF) == NAND_ID_MULTIPLANE)

/* NAND Command Macros */

#define NAND_READ_0(nBytes, bufSelect, device, doEcc, intrDone)                               \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) |                                      \
    NAND_CTRL_UNK28 | NAND_CTRL_UNK27 | NAND_CTRL_UNK26 | NAND_CTRL_UNK25 | NAND_CTRL_UNK24 | \
    NAND_CTRL_CMD(NAND_CMD_READ_0) |                                                          \
    NAND_CTRL_UNK15 | NAND_CTRL_BUFSEL(bufSelect) | NAND_CTRL_DEVICE(device) |                \
    ((doEcc) ? NAND_CTRL_ECC : 0) | NAND_CTRL_XFER_LEN(nBytes))

#define NAND_ERASE_PAGE(device, intrDone)                 \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) |  \
    NAND_CTRL_UNK27 | NAND_CTRL_UNK26 | NAND_CTRL_UNK25 | \
    NAND_CTRL_CMD(NAND_CMD_ERASE_PAGE) |                  \
    NAND_CTRL_DEVICE(device) | NAND_CTRL_MULTICYCLE)

#define NAND_EXEC_ERASE(device, intrDone)                \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) | \
    NAND_CTRL_CMD(NAND_CMD_EXEC_ERASE) |                 \
    NAND_CTRL_UNK15 | NAND_CTRL_DEVICE(device))

#define NAND_READ_ID(bufSelect, device, intrDone)        \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) | \
    NAND_CTRL_UNK28 | NAND_CTRL_UNK24 |                  \
    NAND_CTRL_CMD(NAND_CMD_READ_ID) |                    \
    NAND_CTRL_DEVICE(device) |                           \
    NAND_CTRL_XFER_LEN(4))

#define NAND_READ_STATUS(bufSelect, device, intrDone)        \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) |     \
    NAND_CTRL_UNK28 |                                        \
    NAND_CTRL_CMD(NAND_CMD_READ_STATUS) |                    \
    NAND_CTRL_BUFSEL(bufSelect) | NAND_CTRL_DEVICE(device) | \
    NAND_CTRL_XFER_LEN(1))

#define NAND_READ_STATUS_CMD(cmd, bufSelect, device, intrDone) \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) |       \
    NAND_CTRL_UNK28 |                                          \
    NAND_CTRL_CMD(cmd) |                                       \
    NAND_CTRL_BUFSEL(bufSelect) | NAND_CTRL_DEVICE(device) |   \
    NAND_CTRL_XFER_LEN(1))

#define NAND_FILL_PAGE(nBytes, bufSelect, device, doEcc, intrDone)                            \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) |                                      \
    NAND_CTRL_UNK29 | NAND_CTRL_UNK27 | NAND_CTRL_UNK26 | NAND_CTRL_UNK25 | NAND_CTRL_UNK24 | \
    NAND_CTRL_CMD(NAND_CMD_FILL_PAGE) |                                                       \
    NAND_CTRL_BUFSEL(bufSelect) | NAND_CTRL_DEVICE(device) |                                  \
    ((doEcc) ? NAND_CTRL_ECC : 0) | NAND_CTRL_MULTICYCLE | NAND_CTRL_XFER_LEN(nBytes))

#define NAND_WRITE_PAGE(device, intrDone)                \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) | \
    NAND_CTRL_CMD(NAND_CMD_WRITE_PAGE) |                 \
    NAND_CTRL_UNK15 | NAND_CTRL_DEVICE(device))

#define NAND_DUMMY_PAGE(device, intrDone)                \
   (NAND_CTRL_EXEC | ((intrDone) ? NAND_CTRL_INTR : 0) | \
    NAND_CTRL_CMD(NAND_CMD_DUMMY_PAGE) |                 \
    NAND_CTRL_UNK15 | NAND_CTRL_DEVICE(device))

/* Configuration with which to program PI_48_REG with when using ATB. */
#define NAND_ATB_CONFIG(device, doEcc) (NAND_READ_0(0x3FF, 0, device, doEcc, FALSE) & ~NAND_CTRL_EXEC)

#endif
