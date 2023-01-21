#include "PR/os_internal.h"
#include "PR/bcp.h"
#include "memory.h"
#include "macros.h"

#include "../usb/usb.h"

typedef struct /* size=0x2C */ {
    /* 0x000 0x20 */ u32 eepromAddress;
    /* 0x020 0x20 */ u32 eepromSize;
    /* 0x040 0x20 */ u32 flashAddress;
    /* 0x060 0x20 */ u32 flashSize;
    /* 0x080 0x20 */ u32 sramAddress;
    /* 0x0A0 0x20 */ u32 sramSize;
    /* 0x0C0 0x80 */ u32 pakAddress[4];
    /* 0x140 0x20 */ u32 pakSize;
} OSBbStateVector;

typedef struct /* size=0x44 */ {
    /* 0x000 0x160 */ OSBbStateVector state;
    /* 0x160 0x020 */ u32 romBase;
    /* 0x180 0x020 */ s32 tvType;
    /* 0x1A0 0x020 */ u32 memSize;
    /* 0x1C0 0x020 */ u32 errataSize;
    /* 0x1E0 0x020 */ u32 errataAddress;
    /* 0x200 0x020 */ u32 magic;
} OSBbLaunchMetaData;

typedef u16 BbTicketId;

extern u32 __osBbStashMagic;
extern u32 __osBbIsBb;
extern u32 __osBbEepromSize;
extern u32 __osBbPakSize;
extern u32 __osBbFlashSize;
extern u32 __osBbEepromAddress;
extern u32 __osBbPakAddress[4];
extern u32 __osBbFlashAddress;
extern u32 __osBbSramSize;
extern u32 __osBbSramAddress;
extern u32 __osBbStateDirty;
extern char __osBbStateName[16];
extern s32 __osBbPakBindings[4];

static void _wcopy(u32* src, u32* dst, int bytes) {
    int i;
    int num_words = (bytes + 3) >> 2;

    for (i = 0; i < num_words; i++) {
        *(dst++) = *(src++);
    }
}

static void __osBbUnstashState(char* name, BbTicketId* tid, OSBbStateVector* sv, s32* binding, u32* stateDirty) {
    u32 dummy;
    u32 p = 0;

    if (IO_READ(USB_BUFFER_80100(1, p)) == 0xBABE0002) {
        p += ALIGN(sizeof(u32), 4);

        _wcopy(PHYS_TO_K1(USB_BUFFER_80100(1, p)), sv, sizeof(*sv));
        p += ALIGN(sizeof(*sv), 4);

        _wcopy(PHYS_TO_K1(USB_BUFFER_80100(1, p)), binding, sizeof(s32) * 4);
        p += ALIGN(sizeof(s32) * 4, 4);
        
        _wcopy(PHYS_TO_K1(USB_BUFFER_80100(1, p)), name, sizeof(char) * 16);
        p += ALIGN(sizeof(char) * 16, 4);

        _wcopy(PHYS_TO_K1(USB_BUFFER_80100(1, p)), &dummy, sizeof(*tid));
        p += ALIGN(sizeof(*tid), 4);

        *tid = dummy;

        _wcopy(PHYS_TO_K1(USB_BUFFER_80100(1, p)), stateDirty, sizeof(*stateDirty));
    } else {
        bzero(sv, sizeof(*sv));
        binding[0] = binding[1] = binding[2] = binding[3] = -1;
        name[0] = '\0';
        *tid = 0;
        *stateDirty = 0;
    }
}

static void __osBbGameUnstashState(char* name, OSBbStateVector* sv, s32* binding, u32* stateDirty) {
    int i;

    if ((__osBbStashMagic & 0xFFFF0000) == 0xBABE0000) {
        sv->eepromSize = __osBbEepromSize;
        sv->eepromAddress = __osBbEepromAddress;
        sv->flashSize = __osBbFlashSize;
        sv->flashAddress = __osBbFlashAddress;
        sv->sramSize = __osBbSramSize;
        sv->sramAddress = __osBbSramAddress;
        sv->pakSize = __osBbPakSize;
        for (i = 0; i < 4; i++) {
            sv->pakAddress[i] = __osBbPakAddress[i];
        }
        for (i = 0; i < 4; i++) {
            binding[i] = __osBbPakBindings[i];
        }
        memcpy(name, __osBbStateName, sizeof(__osBbStateName));
        *stateDirty = __osBbStateDirty;
    } else {
        bzero(sv, sizeof(*sv));
        binding[0] = binding[1] = binding[2] = binding[3] = -1;
        name[0] = '\0';
        *stateDirty = 0;
    }
}

static void __osBbStashState(char* name, BbTicketId tid, OSBbStateVector* sv, s32* binding, u32 stateDirty) {
    s32 dummy = tid;
    u32 p = 0;

    IO_WRITE(USB_BUFFER_80100(1, p), 0xBABE0002);
    p += ALIGN(sizeof(u32), 4);

    _wcopy(sv, PHYS_TO_K1(USB_BUFFER_80100(1, p)), sizeof(*sv));
    p += ALIGN(sizeof(*sv), 4);

    _wcopy(binding, PHYS_TO_K1(USB_BUFFER_80100(1, p)), sizeof(s32) * 4);
    p += ALIGN(sizeof(s32) * 4, 4);

    _wcopy(name, PHYS_TO_K1(USB_BUFFER_80100(1, p)), sizeof(char) * 16);
    p += ALIGN(sizeof(char) * 16, 4);

    _wcopy(&dummy, PHYS_TO_K1(USB_BUFFER_80100(1, p)), sizeof(tid));
    p += ALIGN(sizeof(tid), 4);

    _wcopy(&stateDirty, PHYS_TO_K1(USB_BUFFER_80100(1, p)), sizeof(stateDirty));
}

static void __osBbClearState(void) {
    IO_WRITE(USB_BUFFER_80100(1, 0), 0);
}

static void __osBbGetStateFilename(char* name, char* stateName) {
    int i;

    for (i = 0; i < 8 && name[i] != '.' && name[i] != '\0'; i++) {
        stateName[i] = name[i];
    }
    stateName[i + 0] = '.';
    stateName[i + 1] = 's';
    stateName[i + 2] = 't';
    stateName[i + 3] = 'a';
    stateName[i + 4] = '\0';
}

#define ALIGN4K(x) ALIGN(x, 0x4000)

static s32 __osBbSaveState(s8* state_name, OSBbStateVector sv, s32* binding, u32 stateDirty) {
    int i;
    int fd;
    u32 state_size;
    u32 off = 0;
    s32 rv;
    u32 block = 0;

    if (state_name == NULL || state_name[0] == '\0') {
        return -1;
    }

    osBbFDelete("state.tmp");

    state_size = ALIGN4K(sv.eepromSize) + ALIGN4K(sv.flashSize) + ALIGN4K(sv.sramSize);

    if (state_size != 0) {
        fd = osBbFCreate("state.tmp", 1, state_size);
        if (fd >= 0) {
            if (sv.eepromSize != 0) {
                osBbFWrite(fd, off, sv.eepromAddress, ALIGN4K(sv.eepromSize));
                block = sv.eepromAddress;
                off += ALIGN4K(sv.eepromSize);
            }
            if (sv.flashSize != 0) {
                osBbFWrite(fd, off, sv.flashAddress, ALIGN4K(sv.flashSize));
                block = sv.flashAddress;
                off += ALIGN4K(sv.flashSize);
            }
            if (sv.sramSize != 0) {
                osBbFWrite(fd, off, sv.sramAddress, ALIGN4K(sv.sramSize));
                block = sv.sramAddress;
            }
            osBbFClose(fd);
            osBbFRename("state.tmp", state_name);
            if (stateDirty == 0) {
                osBbSignFile(state_name, block);
            }
        }
    }
    osBbFDelete("state.tmp");

    for (i = 0; i < 4; i++) {
        if (binding[i] != -1 && sv.pakAddress[i] != 0) {
            fd = osBbFCreate("state.tmp", 1, ALIGN4K(sv.pakSize));
            if (fd >= 0) {
                char pak_name[16];
                u32 sign;

                osBbFWrite(fd, 0, sv.pakAddress[i], ALIGN4K(sv.pakSize));
                osBbFClose(fd);
                sign = binding[i] & 0x80000000;
                binding[i] &= ~0x80000000;
                sprintf(&pak_name, "%ld.pak", binding[i]);
                osBbFRename("state.tmp", &pak_name);
                if (!sign) {
                    osBbSignFile(&pak_name, sv.pakAddress[i]);
                }
            }
        }
    }
    return 0;
}

s32 osBbSaveState(char* stateName, BbTicketId* tid) {
    char state_name[16];
    OSBbStateVector sv;
    s32 binding[4];
    u32 stateDirty = 0;
    int rv;
    int i;

    stateName[0] = '\0';
    *tid = 0;

    __osBbUnstashState(state_name, tid, &sv, binding, &stateDirty);
    if (state_name[0] == '\0') {
        return -1;
    }

    for (i = 0; state_name[i] != '\0' && i < 16; i++) {
        stateName[i] = state_name[i];
    }

    rv = __osBbSaveState(state_name, sv, binding, stateDirty);
    __osBbClearState();
    return rv;
}

s32 osBbGameCommitState(void) {
    char state_name[16];
    OSBbStateVector sv;
    s32 rv;
    s32 binding[4];
    u32 stateDirty = 0;
    u32 old = IO_READ(PI_48_REG);

    __osBbGameUnstashState(state_name, &sv, binding, &stateDirty);
    rv = __osBbSaveState(state_name, sv, binding, stateDirty);
    IO_WRITE(PI_48_REG, old);
    return rv;
}

void osBbInitState(char* name) {
    int i;

    __osBbStashMagic = 0xBABE0002;

    memcpy(__osBbStateName, name, 16);

    __osBbEepromSize = 0;
    __osBbEepromAddress = 0;
    __osBbFlashSize = 0;
    __osBbFlashAddress = 0;
    __osBbSramSize = 0;
    __osBbSramAddress = 0;
    __osBbPakSize = 0;

    for (i = 0; i < 4; i++) {
        __osBbPakAddress[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        __osBbPakBindings[i] = -1;
    }
    
    __osBbStateDirty = 0;
    __osBbClearState();
}

s32 osBbLoadState(char* name, BbTicketId tid, OSBbStateVector* sv, s32* binding) {
    s32 rv = 0;
    char state_name[16];
    char pak_name[16];
    int i;
    int fd;
    u32 off = 0;
    u8* block;
    u32 stateDirty = 0;

    bzero(state_name, 16);
    __osBbGetStateFilename(name, state_name);
    __osBbStashMagic = 0xBABE0002;

    memcpy(__osBbStateName, state_name, 16);

    __osBbEepromSize = sv->eepromSize;
    __osBbEepromAddress = sv->eepromAddress;
    __osBbFlashSize = sv->flashSize;
    __osBbFlashAddress = sv->flashAddress;
    __osBbSramSize = sv->sramSize;
    __osBbSramAddress = sv->sramAddress;

    if ((block = (void*)__osBbEepromAddress, block != 0) ||
        (block = (void*)__osBbFlashAddress, block != 0) ||
        (block = (void*)__osBbSramAddress, block != 0)) {

        fd = osBbFOpen(state_name, "r");
        if (fd >= 0) {
            if (osBbVerifyFile(state_name, block) == 0) {
                stateDirty = 1;
            }
            if (sv->eepromSize != 0) {
                osBbFRead(fd, off, sv->eepromAddress, ALIGN4K(sv->eepromSize));
                off += ALIGN4K(sv->eepromSize);
            }
            if (sv->flashSize != 0) {
                osBbFRead(fd, off, sv->flashAddress, ALIGN4K(sv->flashSize));
                off += ALIGN4K(sv->flashSize);
            }
            if (sv->sramSize != 0) {
                osBbFRead(fd, off, sv->sramAddress, ALIGN4K(sv->sramSize));
            }
            osBbFClose(fd);
        } else {
            if (sv->eepromSize != 0) {
                bzero(sv->eepromAddress, sv->eepromSize);
            }
            if (sv->flashSize != 0) {
                bzero(sv->flashAddress, sv->flashSize);
            }
            if (sv->sramSize != 0) {
                bzero(sv->sramAddress, sv->sramSize);
            }
        }
    }
    __osBbStateDirty = stateDirty;

    __osBbPakSize = sv->pakSize;
    for (i = 0; i < 4; i++) {
        __osBbPakBindings[i] = binding[i];

        if (binding[i] != -1 && sv->pakAddress[i] != 0) {
            __osBbPakAddress[i] = sv->pakAddress[i];

            sprintf(pak_name, "%ld.pak", binding[i]);

            fd = osBbFOpen(pak_name, "r");
            if (fd >= 0) {
                if (osBbVerifyFile(pak_name, sv->pakAddress[i]) == 0) {
                    binding[i] |= 0x80000000;
                }
                osBbFRead(fd, 0, sv->pakAddress[i], ALIGN4K(sv->pakSize));
            } else {
                OSPfs pfs;

                bzero(&pfs, sizeof(pfs));
                pfs.channel = i;
                bzero(sv->pakAddress[i], sv->pakSize);
                __osGetId(&pfs);
            }
        } else {
            __osBbPakAddress[i] = 0;
        }
    }
    __osBbStashState(state_name, tid, sv, binding, stateDirty);
    return 0;
}

s32 osBbGetLaunchMetaData(OSBbLaunchMetaData* md, u16* blockList, s32 listSize) {
    int i;
    u32* p = (u32*)md;
    OSPiHandle* handler;
    s32 rv = 0;
    u16 lastBlock[2];

    // create a block list containing only the last block of the input block list
    lastBlock[0] = blockList[listSize - 1];
    lastBlock[1] = 0;

    // map the block to the start of PI Domain 1 Address 2
    rv = osBbAtbSetup(PI_DOM1_ADDR2, lastBlock, ARRLEN(lastBlock));
    if (rv < 0) {
        goto label;
    }

    // obtain a cart PI handle
    handler = osCartRomInit();

    // ?
    IO_WRITE(PI_50_REG, 0);
    IO_WRITE(PI_44_REG, 0);
    IO_WRITE(PI_48_REG, 0x1F008BFF);

    // Read the CMD from the end of the block with programmed IO reads
    for (i = 0x4000 - sizeof(*md); i < 0x4000; i += 4) {
        osEPiReadIo(handler, i, p++);
    }

    // Check magic number
    if ((md->magic & ~0xFF) != 0x43414D00/* "MAC" */) {
        rv = -1;
    }

    if (rv < 0) {
label:
        // Could not read CMD, create a "default" one?
        bzero(md, sizeof(*md));
        md->memSize = 0x400000;
        md->tvType = OS_TV_NTSC;
        md->romBase = PHYS_TO_K1(PI_DOM1_ADDR2);
    }
    return rv;
}
