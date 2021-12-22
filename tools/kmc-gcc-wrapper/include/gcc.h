// GCC.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/GCC.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x1000000;
uintptr_t startAddr = 0x01008a44;
int fileOffset = 0x1000;
int codeDataLength = 0x10358 + 0x16d0;
size_t bssSize = 0x01011a28;
uintptr_t mallocAddr = 0x0100b898;
uintptr_t reallocAddr = 0x0100bae0;
uintptr_t environAddr = 0x01011d80;
uintptr_t _unix2dosnameAddr = 0x0100c00c;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x0100a5c0,
    0x0100a5cd,
    0x0100a5e0,
    0x0100a5fe,
    0x0100a60f,
    0x0100a627,
    0x0100a63f,
    0x0100a65e,
    0x0100a67b,
    0x0100a692,
    0x0100a6ab,
    0x0100a6ba,
    0x0100a6d1,
    0x0100a6e7,
    0x0100a707,
    0x0100a733,
    0x0100a762,
    0x0100a776,
    0x0100a78a,
    0x0100a79e,
    0x0100a7a6,
    0x0100a7bf,
    0x0100a7d7,
    0x0100a7e6,
    0x0100a7fa,
    0x0100a81b,
    0x0100a82a,
    0x0100a847,
    0x0100a85f,
    0x0100a8a1,
    0x0100a8cb,
    0x0100a8f6,
};

#define REDIRECT_SYSTEM
uintptr_t systemAddr = 0x0100e528;

// Extra stuff for GCC specifically
#define IS_GCC
uintptr_t mktempAddr = 0x0100e088;
uintptr_t unlinkAddr = 0x0100dd90;
