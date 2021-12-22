#ifndef __DOS_H__
#define __DOS_H__

#include <stdint.h>
#include "log.h"

typedef enum {
    DOS_ERR_FILE_NOT_FOUND = 0x02,
    DOS_ERR_PATH_NOT_FOUND = 0x03,
    DOS_ERR_INVALID_HANDLE = 0x06,
    DOS_ERR_INVALID_ACCESS_MODE = 0x0C,
    DOS_ERR_SEEK = 0x19,
} dos_error_t;

typedef enum {
    DOS_SET_DTA = 0x1A,
    DOS_GET_VER = 0x30,
    DOS_CREATE_FILE = 0x3C,
    DOS_OPEN_FILE = 0x3D,
    DOS_CLOSE_FILE = 0x3E,
    DOS_READ = 0x3F,
    DOS_WRITE = 0x40,
    DOS_DELETE = 0x41,
    DOS_SEEK = 0x42,
    DOS_FILE_ATTRIB = 0x43,
    DOS_IOCTL = 0x44,
    DOS_GET_CWD = 0x47,
    DOS_EXIT = 0x4C,
    DOS_FIND_FILE = 0x4E,
    DOS_GET_SET_TIMESTAMP = 0x57,
} dos_api_t;

typedef enum {
    DOS_FILE_READ = 0,
    DOS_FILE_WRITE = 1,
    DOS_FILE_READWRITE = 2,
} dos_file_mode_t;

typedef enum {
    DOS_IOCTL_GET_INFO = 0,
    DOS_IOCTL_SET_INFO = 1,
    DOS_IOCTL_READ_CHAR_DEV = 2,
    DOS_IOCTL_WRITE_CHAR_DEV = 3,
    DOS_IOCTL_READ_BLOCK_DEV = 4,
    DOS_IOCTL_WRITE_BLOCK_DEV = 5,
} dos_ioctl_t;

typedef enum {
    DOS_ATTRIB_GET = 0,
    DOS_ATTRIB_SET = 1,
} dos_file_attrib_t;

typedef enum {
    DOS_TIMESTAMP_GET = 0,
    DOS_TIMESTAMP_SET = 1,
} dos_timestamp_action_t;

typedef struct {
    uint32_t *eax;
    uint32_t *ebx;
    uint32_t *ecx;
    uint32_t *edx;
    uint32_t *efl;
    uint32_t *esi;
    int8_t ah;
    int8_t al;
} context_t;

// Set carry flag
#define DOS_SET_ERROR(ctx) *(ctx)->efl |= 0x0001
// Clear carry flag
#define DOS_CLEAR_ERROR(ctx) *(ctx)->efl &= ~0x0001

#define DOS_RETURN(ctx, val) \
{ \
    int val_ = (int)(val); \
    LOG_PRINT("  returning 0x%X\n", (int)(val_)); \
    *(ctx)->eax = (val_); \
    return; \
}

#define NUM_HANDLES 0x100
#define MAX_FILENAME_LEN 0x100
#define DOS_MAX_PATH 64

void dos_21h_handler(context_t *ctx);
void dos_init(void);

#endif