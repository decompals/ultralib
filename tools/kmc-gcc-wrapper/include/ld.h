// LD.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/LD.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x1080000;
uintptr_t startAddr = 0x0108eee8;
int fileOffset = 0x1000;
int codeDataLength = 0x3f460 + 0x1488;
size_t bssSize = 0x010c08e8;
uintptr_t mallocAddr = 0x010bac90;
uintptr_t reallocAddr = 0x010baed8;
uintptr_t environAddr = 0x010c1048;
uintptr_t _unix2dosnameAddr = 0x010bb404;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = { // TODO
    0x010b990d,
    0x010b9920,
    0x010b993e,
    0x010b994f,
    0x010b9967,
    0x010b997f,
    0x010b999e,
    0x010b99bb,
    0x010b99d2,
    0x010b99eb,
    0x010b99fa,
    0x010b9a11,
    0x010b9a27,
    0x010b9a47,
    0x010b9a73,
    0x010b9aa2,
    0x010b9ab6,
    0x010b9aca,
    0x010b9ade,
    0x010b9ae6,
    0x010b9aff,
    0x010b9b17,
    0x010b9b26,
    0x010b9b3a,
    0x010b9b5b,
    0x010b9b6a,
    0x010b9b87,
    0x010b9b9f,
    0x010b9be1,
    0x010b9c0b,
    0x010b9c36,
};
