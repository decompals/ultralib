#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>

#include "dos.h"
#include "log.h"

FILE *fileHandles[NUM_HANDLES]; // Should be plenty
char filenames[NUM_HANDLES][MAX_FILENAME_LEN];

// Start at nonzero to skip the default file handles
int numFileHandles = 10;

#define VALID_HANDLE(handle) ((handle) >= 0 && (handle) < NUM_HANDLES && fileHandles[(handle)] != NULL)

#define VALID_HANDLE_CHECK(ctx, handle) \
    if (!VALID_HANDLE(handle)) \
    { \
        LOG_PRINT("  invalid handle\n"); \
        DOS_SET_ERROR(ctx); \
        DOS_RETURN(ctx, DOS_ERR_INVALID_HANDLE); \
    }

void replace_backslashes(char *input, int len)
{
    while (len > 0)
    {
        if (*input == '\\')
        {
            *input = '/';
        }
        input++;
        len--;
    }
}

void replace_forwardslashes(char *input, int len)
{
    while (len > 0)
    {
        if (*input == '/')
        {
            *input = '\\';
        }
        input++;
        len--;
    }
}

const char progsToReplace[] = "cc1.out as.out ld.out cpp.out mild.out";

void remove_out_extension(char *input)
{
    // Remove the out extension only on specific programs that can be run
    if (strstr(progsToReplace, input))
    {
        char *outsubstr = strstr(input, ".out");
        if (outsubstr != NULL)
        {
            outsubstr[0] = 0;
            outsubstr[1] = 0;
            outsubstr[2] = 0;
            outsubstr[3] = 0;
        }
    }
}

void dos_open_file(context_t *ctx)
{
    const char *filename = (const char*)*ctx->edx;
    int filemode = ctx->al;
    int filenameLen = strnlen(filename, FILENAME_MAX);
    char filenameReplaced[filenameLen + 1]; // After replacing backslashes with forward slashes
    char *modestr = NULL;
    switch (filemode)
    {
        case DOS_FILE_READ:
            modestr = "rb";
            break;
        case DOS_FILE_WRITE:
            modestr = "wb";
            break;
        case DOS_FILE_READWRITE:
            modestr = "rb+";
            break;
    }
    if (modestr == NULL)
    {
        LOG_PRINT("  invalid access code\n");
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_INVALID_ACCESS_MODE); // access code invalid
    }
    if (filenameLen == FILENAME_MAX)
    {
        LOG_PRINT("  filename too long: %s\n", filename);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    filenameReplaced[filenameLen] = 0;
    memcpy(filenameReplaced, filename, filenameLen);
    replace_backslashes(filenameReplaced, filenameLen);
    remove_out_extension(filenameReplaced); // redirects x.out to x for running programs


    if (filemode == DOS_FILE_READ && (access(filenameReplaced, F_OK) != 0))
    {
        LOG_PRINT("  file does not exist: %s\n", filenameReplaced);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    LOG_PRINT("  open file: %s\n", filenameReplaced);
    for (int i = 0; i < NUM_HANDLES; i++)
    {
        if (fileHandles[i] && strcmp(filenameReplaced, filenames[i]) == 0)
        {
            LOG_PRINT("    returning existing file handle: %d\n", i);
            fseek(fileHandles[i], 0, SEEK_SET);
            DOS_CLEAR_ERROR(ctx);
            DOS_RETURN(ctx, i);
        }
    }
    fileHandles[numFileHandles] = fopen(filenameReplaced, modestr);
    strncpy(filenames[numFileHandles], filenameReplaced, MAX_FILENAME_LEN);
    LOG_PRINT("    returned file handle: %d\n", numFileHandles);
    LOG_PRINT("    clearing carry flag\n");
    DOS_CLEAR_ERROR(ctx); // Clear carry flag
    DOS_RETURN(ctx, numFileHandles++);
}

void dos_create_file(context_t *ctx)
{
    const char *filename = (const char*)*ctx->edx;
    int filenameLen = strnlen(filename, FILENAME_MAX);
    char filenameReplaced[filenameLen + 1]; // After replacing backslashes with forward slashes
    if (filenameLen == FILENAME_MAX)
    {
        LOG_PRINT("  filename too long: %s\n", filename);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    filenameReplaced[filenameLen] = 0;
    memcpy(filenameReplaced, filename, filenameLen);
    replace_backslashes(filenameReplaced, filenameLen);
    LOG_PRINT("  create file: %s\n", filenameReplaced);
    for (int i = 0; i < NUM_HANDLES; i++)
    {
        if (fileHandles[i] && strcmp(filenameReplaced, filenames[i]) == 0)
        {
            LOG_PRINT("    returning existing file handle: %d\n", i);
            fseek(fileHandles[i], 0, SEEK_SET);
            DOS_CLEAR_ERROR(ctx);
            DOS_RETURN(ctx, i);
        }
    }
    fileHandles[numFileHandles] = fopen(filenameReplaced, "wb");
    strncpy(filenames[numFileHandles], filenameReplaced, MAX_FILENAME_LEN);
    LOG_PRINT("    returned file handle: %d\n", numFileHandles);
    LOG_PRINT("    clearing carry flag\n");
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, numFileHandles++);
}

void dos_ioctl(context_t *ctx)
{
    int ioctlFunc = ctx->al;
    int handle = (*ctx->ebx) & 0xFFFF;
    __attribute__((unused)) int devNum = (*ctx->ebx) & 0xFF;
    __attribute__((unused)) int numBytes = *ctx->ecx;
    __attribute__((unused)) void *buf = (void*)*ctx->edx;

    LOG_PRINT("  ioctl: function 0x%02X\n", ioctlFunc);

    switch (ioctlFunc)
    {
        case DOS_IOCTL_GET_INFO:
            {
                VALID_HANDLE_CHECK(ctx, handle);
                uint32_t devInfo = 0;
                // uint32_t devInfo = 0b0100100010110000;
                // uint32_t devInfo = 0xFFFF;
                LOG_PRINT("    devinfo: %u\n", devInfo);
                *ctx->edx = devInfo; // TODO real values
                DOS_CLEAR_ERROR(ctx);
                DOS_RETURN(ctx, devInfo);
            }
            break;
    }
}

void dos_write(context_t *ctx)
{
    int handle = (*ctx->ebx) & 0xFFFF;
    int numBytes = *ctx->ecx;
    const char *bytes = (const char*)*ctx->edx;
    int bytesWritten;

    LOG_PRINT("  write 0x%X bytes to handle: %d\n", numBytes, handle);
    LOG_PRINT("    %.*s\n", numBytes, bytes);

    VALID_HANDLE_CHECK(ctx, handle);

    bytesWritten = fwrite(bytes, 1, numBytes, fileHandles[handle]);
    fflush(fileHandles[handle]);
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, bytesWritten);
}

void dos_delete(context_t *ctx)
{
    const char *filename = (const char*)*ctx->edx;
    int filenameLen = strnlen(filename, FILENAME_MAX);
    char filenameReplaced[filenameLen + 1]; // After replacing backslashes with forward slashes

    if (filenameLen == FILENAME_MAX)
    {
        LOG_PRINT("  filename too long: %s\n", filename);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    filenameReplaced[filenameLen] = 0;
    memcpy(filenameReplaced, filename, filenameLen);
    replace_backslashes(filenameReplaced, filenameLen);
    LOG_PRINT("  deleting file: %s\n", filenameReplaced);

    if (remove(filenameReplaced) != 0)
    {
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // Technically could be another reason but w/e
    }
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0);
}

void dos_seek(context_t *ctx)
{
    int whence = ctx->al;
    int handle = (*ctx->ebx) & 0xFFFF;
    uint64_t offset = ((uint64_t)*ctx->ecx) << 16 | (*ctx->edx);
    long pos;

    LOG_PRINT("  seeking file handle %d to 0x%016llX bytes from whence %d\n", handle, offset, whence);

    VALID_HANDLE_CHECK(ctx, handle);

    if (fseek(fileHandles[handle], offset, whence) != 0)
    {
        LOG_PRINT("  failed to seek\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_SEEK);
    }

    pos = ftell(fileHandles[handle]);


    DOS_CLEAR_ERROR(ctx);
    *ctx->edx = (pos >> 16) & 0xFFFF;
    DOS_RETURN(ctx, pos & 0xFFFF);
}

const char testdata[] = ".set noreorder\naddiu $2, $4, 2\njr $31\nnop\n";

void dos_read(context_t *ctx)
{
    int handle = (*ctx->ebx) & 0xFFFF;
    int numBytes = *ctx->ecx;
    void *out = (void**)*ctx->edx;
    int bytesRead;

    VALID_HANDLE_CHECK(ctx, handle);

    LOG_PRINT("  reading 0x%X bytes into address 0x%08X from handle: %d\n", numBytes, (uint32_t)out, handle);

    bytesRead = fread(out, 1, numBytes, fileHandles[handle]);
    LOG_PRINT("    %.*s\n", numBytes, (const char*)out);
    
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, bytesRead);
}

void dos_close_file(context_t *ctx)
{
    int handle = (*ctx->ebx) & 0xFFFF;

    if ((handle) < 0 || (handle) >= NUM_HANDLES)
    {
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_INVALID_HANDLE);
    }
    if (fileHandles[handle])
    {
        LOG_PRINT("    closing and clearing file handle: %d\n", handle);
        fclose(fileHandles[handle]);
        filenames[handle][0] = 0x00;
        fileHandles[handle] = NULL;
    }
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0x00);
}

void dos_file_attrib(context_t *ctx)
{
    dos_file_attrib_t action = ctx->al;
    const char *filename = (const char*)*ctx->edx;
    int filenameLen = strnlen(filename, FILENAME_MAX);
    char filenameReplaced[filenameLen + 1]; // After replacing backslashes with forward slashes
    if (filenameLen == FILENAME_MAX)
    {
        LOG_PRINT("  filename too long: %s\n", filename);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    filenameReplaced[filenameLen] = 0;
    memcpy(filenameReplaced, filename, filenameLen);
    replace_backslashes(filenameReplaced, filenameLen);
    LOG_PRINT("  get/set attrib file: action %d file %s\n", action, filenameReplaced);

    switch (action)
    {
        case DOS_ATTRIB_GET:
            if (access(filenameReplaced, F_OK) != 0)
            {
                LOG_PRINT("  file not found\n");
                DOS_SET_ERROR(ctx);
                DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND);
            }
            DOS_CLEAR_ERROR(ctx);
            DOS_RETURN(ctx, 0x00); // TODO read actual attributes
            break;
        case DOS_ATTRIB_SET:
            LOG_PRINT("  unimplemented\n");
            DOS_CLEAR_ERROR(ctx);
            DOS_RETURN(ctx, 0x00); // TODO actually implement this
            break;
    }
    
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0x00);
}

uint32_t dta = 0;

void dos_set_dta(context_t *ctx)
{
    LOG_PRINT("  setting DTA to 0x%08X\n", *ctx->edx);
    dta = *ctx->edx;
}

void dos_find_file(context_t *ctx)
{
    const char *filename = (const char*)*ctx->edx;
    int filenameLen = strnlen(filename, FILENAME_MAX);
    char filenameReplaced[filenameLen + 1]; // After replacing backslashes with forward slashes
    if (filenameLen == FILENAME_MAX)
    {
        LOG_PRINT("  filename too long: %s\n", filename);
        LOG_PRINT("    setting carry flag\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_FILE_NOT_FOUND); // does not exist
    }
    filenameReplaced[filenameLen] = 0;
    memcpy(filenameReplaced, filename, filenameLen);
    replace_backslashes(filenameReplaced, filenameLen);
    LOG_PRINT("  find filename %s\n", filenameReplaced); // TODO actually implement this
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0);
}

void dos_get_cwd(context_t *ctx)
{
    char *pathOut = (char*)*ctx->esi;
    char cwd[DOS_MAX_PATH + 1];
    LOG_PRINT("  get cwd\n");
    if (getcwd(cwd, DOS_MAX_PATH) == NULL)
    {
        LOG_PRINT("    get cwd error\n");
        DOS_SET_ERROR(ctx);
        DOS_RETURN(ctx, DOS_ERR_PATH_NOT_FOUND); // TODO is this the right error?
    }
    replace_forwardslashes(cwd, DOS_MAX_PATH + 1);
    strncpy(pathOut, cwd + 1, DOS_MAX_PATH); // Omit the leading forward slash
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0);
}

void dos_get_set_timestamp(context_t *ctx)
{
    dos_timestamp_action_t action = ctx->al;
    int handle = (*ctx->ebx) & 0xFFFF;
    int fd;
    struct stat details;
    struct tm *time;
    int timeRet = 0;
    int dateRet = 0;

    LOG_PRINT("  get/set timestamp: action %d handle %d\n", action, handle);
    VALID_HANDLE_CHECK(ctx, handle);

    fd = fileno(fileHandles[handle]);

    switch (action)
    {
        case DOS_TIMESTAMP_GET:
            fstat(fd, &details);
            time = localtime(&details.st_mtime);
            timeRet |= (time->tm_sec / 2)   << 0;
            timeRet |= (time->tm_min)       << 5;
            timeRet |= (time->tm_hour)      << 11;
            dateRet |= (time->tm_mday)      << 0;
            dateRet |= (time->tm_mon)       << 5;
            dateRet |= (time->tm_year - 80) << 9;
            DOS_CLEAR_ERROR(ctx);
            DOS_RETURN(ctx, 0x00); // TODO read actual attributes
            break;
        case DOS_TIMESTAMP_SET:
        default:
            LOG_PRINT("  unimplemented\n");
            exit(EXIT_FAILURE);
            break;
    }
    
    DOS_CLEAR_ERROR(ctx);
    DOS_RETURN(ctx, 0x00);
}

#define DOS_HANDLER(ctx, value, handler) \
    case value: \
        handler(ctx); \
        break

void dos_21h_handler(context_t *ctx)
{        
    switch (ctx->ah)
    {
        case DOS_GET_VER:
            *ctx->eax = 0x00000006; // DOS version 6
            break;
        DOS_HANDLER(ctx, DOS_CREATE_FILE, dos_create_file);
        DOS_HANDLER(ctx, DOS_OPEN_FILE, dos_open_file);
        DOS_HANDLER(ctx, DOS_IOCTL, dos_ioctl);
        DOS_HANDLER(ctx, DOS_WRITE, dos_write);
        DOS_HANDLER(ctx, DOS_DELETE, dos_delete);
        DOS_HANDLER(ctx, DOS_SEEK, dos_seek);
        DOS_HANDLER(ctx, DOS_READ, dos_read);
        DOS_HANDLER(ctx, DOS_CLOSE_FILE, dos_close_file);
        DOS_HANDLER(ctx, DOS_FILE_ATTRIB, dos_file_attrib);
        DOS_HANDLER(ctx, DOS_SET_DTA, dos_set_dta);
        DOS_HANDLER(ctx, DOS_FIND_FILE, dos_find_file);
        DOS_HANDLER(ctx, DOS_GET_CWD, dos_get_cwd);
        DOS_HANDLER(ctx, DOS_GET_SET_TIMESTAMP, dos_get_set_timestamp);
        case DOS_EXIT:
            exit(ctx->al);
            break;
        default:
            LOG_PRINT("Unimplemented system call: 0x%02X\n", ctx->ah);
            exit(EXIT_FAILURE);
            break;
    }
}

FILE *logFile;

void dos_init(void)
{
#ifndef NDEBUG
    logFile = fopen("log.txt", "w");
#endif
    memset(fileHandles, 0, sizeof(fileHandles));
    fileHandles[0] = stdin;
    fileHandles[1] = stdout;
    fileHandles[2] = stderr;
}

