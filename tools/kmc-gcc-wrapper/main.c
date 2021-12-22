#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "dos.h"
#include "log.h"

#ifdef __APPLE__
#define _XOPEN_SOURCE 600
#endif
#include <ucontext.h>

// Variable include
#define INCLUDE_PROG() <PROG.h>
#include INCLUDE_PROG()

typedef __attribute__((__cdecl__)) void (func_t)();
typedef __attribute__((__cdecl__)) const char* (getenv_t)(const char*);


#define NUM_NOPS (sizeof(nops) / sizeof(nops[0]))
#define NUM_INT21 (sizeof(int21Addrs) / sizeof(int21Addrs[0]))

#define INT3 0xCC
#define NOP 0x90

char *programPath;

// SIGTRAP handler that creates the context passed to the DOS 21h handler
void sig_handler(__attribute__((unused)) int signum, __attribute__((unused)) siginfo_t *info, void *vcontext)
{
    ucontext_t *context = (ucontext_t*)vcontext;
    #ifdef __APPLE__
    uint32_t *eax = (uint32_t *)&context->uc_mcontext->__ss.__eax;
    uint32_t *ebx = (uint32_t *)&context->uc_mcontext->__ss.__ebx;
    uint32_t *ecx = (uint32_t *)&context->uc_mcontext->__ss.__ecx;
    uint32_t *edx = (uint32_t *)&context->uc_mcontext->__ss.__edx;
    uint32_t *efl = (uint32_t *)&context->uc_mcontext->__ss.__eflags;
    uint32_t *esi = (uint32_t *)&context->uc_mcontext->__ss.__esi;
    #else
    uint32_t *eax = (uint32_t *)&context->uc_mcontext.gregs[REG_EAX];
    uint32_t *ebx = (uint32_t *)&context->uc_mcontext.gregs[REG_EBX];
    uint32_t *ecx = (uint32_t *)&context->uc_mcontext.gregs[REG_ECX];
    uint32_t *edx = (uint32_t *)&context->uc_mcontext.gregs[REG_EDX];
    uint32_t *efl = (uint32_t *)&context->uc_mcontext.gregs[REG_EFL];
    uint32_t *esi = (uint32_t *)&context->uc_mcontext.gregs[REG_ESI];
    #endif
#ifndef NDEBUG
    unsigned long ip = context->uc_mcontext.gregs[REG_EIP];
    unsigned long ds = context->uc_mcontext.gregs[REG_DS];
    uint32_t *sp = (uint32_t*)context->uc_mcontext.gregs[REG_ESP];
#endif
    unsigned long ah = ((*eax) >> 8) & 0xFF;
    unsigned long al = ((*eax) >> 0) & 0xFF;
    context_t ctx = {
        .eax = eax,
        .ebx = ebx,
        .ecx = ecx,
        .edx = edx,
        .efl = efl,
        .esi = esi,
        .ah = ah,
        .al = al
    };
    
    LOG_PRINT("DOS syscall\n"
           "  IP:  0x%08lX\n"
           "  AH:  0x%02lX\n"
           "  AL:  0x%02lX\n"
           "  EBX: 0x%08X\n"
           "  ECX: 0x%08X\n"
           "  EDX: 0x%08X\n"
           "  DS:  0x%04lX\n", 
           ip, ah, al, *ebx, *ecx, *edx, ds);
    LOG_PRINT("  stack\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n"
           "    0x%08X 0x%08X 0x%08X 0x%08X\n",
           sp[0],  sp[1],  sp[2],  sp[3],
           sp[4],  sp[5],  sp[6],  sp[7],
           sp[8],  sp[9],  sp[10], sp[11],
           sp[12], sp[13], sp[14], sp[15],
           sp[16], sp[17], sp[18], sp[19],
           sp[20], sp[21], sp[22], sp[23],
           sp[24], sp[25], sp[26], sp[27],
           sp[28], sp[29], sp[30], sp[31],
           sp[32], sp[33], sp[34], sp[35],
           sp[36], sp[37], sp[38], sp[39],
           sp[40], sp[41], sp[42], sp[43],
           sp[44], sp[45], sp[46], sp[47]);

    dos_21h_handler(&ctx);
}

// Wrapper for malloc that will be jumped to via a patch on the original binary's malloc
__attribute__((__cdecl__)) void *malloc_wrapper(size_t len)
{
    return malloc(len);
}

// Wrapper for malloc that will be jumped to via a patch on the original binary's malloc
__attribute__((__cdecl__)) void *realloc_wrapper(void *ptr, size_t len)
{
    return realloc(ptr, len);
}

// Wrapper for an annoying function that "converts" paths to DOS valid ones
__attribute__((__cdecl__)) char *_unix2dosname_wrapper(char *in)
{
#ifdef IS_AS
    char *dotPos = strrchr(in, '.');
    if (dotPos != NULL)
    {
        *(char**)(dos_ext) = dotPos + 1;
    }
#endif
    return in; // That'll show em
}

#if defined(REDIRECT_SYSTEM) || defined(REDIRECT_SPAWNVPE)
// TODO make this function instead escape the parenthesis in backslashes
void replace_parens(char *str)
{
    char *openIndex = strchr(str, '(');

    if (openIndex != NULL)
    {
        while (*str)
        {
            *str = ' ';
            str++;
        }
    }
}

// Caller-freed memory!
char *build_call_string(__attribute__((unused))char *cmd, char *argv[])
{
    char *cmdString;
    char **curArg = argv;
    int cmdLen = 3;

    if (cmd[0] == 'c' && cmd[1] == 'p' && cmd[2] == 'p')
    {
        cmdString = malloc(strlen("mips-linux-gnu-") + 1);
        strcpy(cmdString, "mips-linux-gnu-");
        cmdLen = strlen("mips-linux-gnu-")  + 1;
    }
    else
    {
        cmdString = malloc(strlen(programPath) + 1);
        strcpy(cmdString, programPath);
        cmdLen = strlen(programPath) + 1;
    }
    while (*curArg)
    {
        int curArgLen = strlen(*curArg);
        cmdString = (char *)realloc(cmdString, cmdLen + curArgLen + 1);
        strcat(cmdString, *curArg);
        strcat(cmdString, " ");
        replace_parens(cmdString + cmdLen - 1);
        cmdLen += curArgLen + 1;
        curArg++;
    }
    // puts(cmdString);

    return cmdString;
}
#endif

#ifdef REDIRECT_SYSTEM
__attribute__((__cdecl__)) int system_wrapper(char *cmd, char *argv[])
{
    int ret;
    char *callString = build_call_string(cmd, argv); 
    LOG_PRINT("system: %s\n", cmd);
    LOG_PRINT("  redirected: %s\n", callString);
    ret = system(callString);
    free(callString);
    return ret;
}
#endif

#ifdef REDIRECT_SPAWNVPE
__attribute__((__cdecl__)) int spawnvpe_wrapper(__attribute__((unused)) int mode, char *cmd, char *argv[], __attribute__((unused)) const char *const *envp)
{
    int ret;
    char *callString = build_call_string(cmd, argv); 
    LOG_PRINT("spawnvpe: %d %s\n", mode, cmd);
    LOG_PRINT("  redirected: %s\n", callString);
    ret = system(callString);
    free(callString);
    return ret;
}
#endif

#ifdef IS_GCC
__attribute__((__cdecl__)) int mktemp_wrapper(char *template)
{
    LOG_PRINT("mktemp %s\n", template);
    return mkstemp(template);
}

__attribute__((__cdecl__)) int unlink_wrapper(char *filename)
{
    char *dotPos = strrchr(filename, '.');
    int ret;
    // Run unlink twice if there's an extension in the filename
    // Why does this need to be done? Who knows, but it fixes leaving files behind
    if (dotPos != NULL)
    {
        char tmpFilename[dotPos - filename + 1];
        memcpy(tmpFilename, filename, dotPos - filename);
        tmpFilename[dotPos - filename] = '\0';
        unlink(tmpFilename);
    }
    ret = unlink(filename);
    return ret;
}
#endif

// Overwrites the first instructions of some functions in the original binary with jumps to our wrappers instead
void write_jump_hooks()
{
    uint32_t mallocWrapperAddr = (uint32_t)&malloc_wrapper;
    uint32_t reallocWrapperAddr = (uint32_t)&realloc_wrapper;
    uint32_t _unix2dosnameWrapperAddr = (uint32_t)&_unix2dosname_wrapper;
#ifdef REDIRECT_SYSTEM
    uint32_t systemWrapperAddr = (uint32_t)&system_wrapper;
#endif
#ifdef IS_GCC
    uint32_t mktempWrapperAddr = (uint32_t)&mktemp_wrapper;
    uint32_t unlinkWrapperAddr = (uint32_t)&unlink_wrapper;
#endif
#ifdef REDIRECT_SPAWNVPE
    uint32_t spawnvpeWrapperAddr = (uint32_t)&spawnvpe_wrapper;
#endif
    uint32_t rel32 = mallocWrapperAddr - (uint32_t)mallocAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)mallocAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)mallocAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)mallocAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)mallocAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)mallocAddr)[4] = (rel32 >> 24) & 0xFF;
    
    rel32 = reallocWrapperAddr - (uint32_t)reallocAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)reallocAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)reallocAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)reallocAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)reallocAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)reallocAddr)[4] = (rel32 >> 24) & 0xFF;
    
    rel32 = _unix2dosnameWrapperAddr - (uint32_t)_unix2dosnameAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)_unix2dosnameAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)_unix2dosnameAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)_unix2dosnameAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)_unix2dosnameAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)_unix2dosnameAddr)[4] = (rel32 >> 24) & 0xFF;

#ifdef REDIRECT_SYSTEM
    rel32 = systemWrapperAddr - (uint32_t)systemAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)systemAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)systemAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)systemAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)systemAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)systemAddr)[4] = (rel32 >> 24) & 0xFF;
#endif
    
#ifdef IS_GCC
    rel32 = mktempWrapperAddr - (uint32_t)mktempAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)mktempAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)mktempAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)mktempAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)mktempAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)mktempAddr)[4] = (rel32 >> 24) & 0xFF;
    
    rel32 = unlinkWrapperAddr - (uint32_t)unlinkAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)unlinkAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)unlinkAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)unlinkAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)unlinkAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)unlinkAddr)[4] = (rel32 >> 24) & 0xFF;
#endif

#ifdef REDIRECT_SPAWNVPE
    rel32 = spawnvpeWrapperAddr - (uint32_t)spawnvpeAddr - 5;
    // x86 jmp rel32
    ((uint8_t*)spawnvpeAddr)[0] = 0xE9;

    // jump offset
    ((uint8_t*)spawnvpeAddr)[1] = (rel32 >>  0) & 0xFF;
    ((uint8_t*)spawnvpeAddr)[2] = (rel32 >>  8) & 0xFF;
    ((uint8_t*)spawnvpeAddr)[3] = (rel32 >> 16) & 0xFF;
    ((uint8_t*)spawnvpeAddr)[4] = (rel32 >> 24) & 0xFF;
#endif
}

FILE *fopen_relative(const char *relativePath, const char *mode)
{
    int absPathLen = strlen(programPath) + strlen(relativePath) + 1;
    char *absPath = malloc(absPathLen);
    FILE *ret;
    strcpy(absPath, programPath);
    strcat(absPath, relativePath);
    ret = fopen(absPath, mode);
    free(absPath);
    return ret;
}

int main(int argc, char* argv[])
{
    FILE *f;
    void *progMem;
    func_t *binStart = (func_t *)startAddr;
    size_t i;
    struct sigaction sig_action;
    char *programPathSlashPos;

    programPath = realpath(argv[0], NULL);
    programPathSlashPos = strrchr(programPath, '/');
    if (programPathSlashPos != NULL)
    {
        *(programPathSlashPos + 1) = 0;
    }
    
    // Set up the SIGTRAP handler
    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_sigaction = sig_handler;
    sig_action.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&sig_action.sa_mask);
    sigaction(SIGTRAP, &sig_action, 0); // Register signal handler

    // Get the length of the input binary
    f = fopen_relative(BIN_FILE, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Error: Cannot open %s\n", BIN_FILE);
        return EXIT_FAILURE;
    }
    
    // mmap a region of memory at the fixed load address for the given binary
    progMem = mmap((void*)loadAddr, codeDataLength + bssSize - fileOffset, PROT_READ | PROT_EXEC | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0x0);
    // Read the program bytes into the mmap'd memory region
    fseek(f, fileOffset, SEEK_SET);
    if (fread(progMem, codeDataLength, 1, f) < 1)
    {
        fprintf(stderr, "Error: Failed to read file contents\n");
        return EXIT_FAILURE;
    }
    fclose(f);

    // Overwrite any instructions that need to be nop'd
    for (i = 0; i < NUM_NOPS; i++)
    {
        *((uint8_t *)nops[i]) = NOP;
    }

    // Overwrite any int 0x21 instructions with int3; nop so they can be caught by the SIGTRAP handler
    for (i = 0; i < NUM_INT21; i++)
    {
        *((uint8_t *)(int21Addrs[i] + 0)) = INT3;
        *((uint8_t *)(int21Addrs[i] + 1)) = NOP;
    }

    // Overwrite the program's environ with the real one
    #ifdef __APPLE__
    extern char **environ;
    #endif
    *(char***)environAddr = environ;

    // Write malloc/realloc jump hooks onto the program's ram
    write_jump_hooks();

    // Initialize any required DOS state
    dos_init();

#ifdef IS_AS
    // Set the program to as
    *(int*)(_kmc_prg_no) = 2;
#endif

    // Call the program's main function
    binStart(argc, argv);

    free(programPath);

    return EXIT_SUCCESS;
}
