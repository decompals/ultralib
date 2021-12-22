// CC1.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/CC1.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x01080000;
uintptr_t startAddr = 0x010a8134;
int fileOffset = 0x1000;
int codeDataLength = 0x128e44 + 0x6160;
size_t bssSize = 0x011aefa4;
uintptr_t mallocAddr = 0x011a4bd8;
uintptr_t reallocAddr = 0x011a4e20;
uintptr_t environAddr = 0x011b6b98;
uintptr_t _unix2dosnameAddr = 0x011a534c;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x011a390d,
    0x011a3920,
    0x011a393e,
    0x011a394f,
    0x011a3967,
    0x011a397f,
    0x011a399e,
    0x011a39bb,
    0x011a39d2,
    0x011a39eb,
    0x011a39fa,
    0x011a3a11,
    0x011a3a27,
    0x011a3a47,
    0x011a3aa2,
    0x011a3ab6,
    0x011a3aca,
    0x011a3ade,
    0x011a3ae6,
    0x011a3aff,
    0x011a3b17,
    0x011a3b26,
    0x011a3b3a,
    0x011a3b5b,
    0x011a3b6a,
    0x011a3b87,
    0x011a3b9f,
    0x011a3be1,
    0x011a3c0b,
    0x011a3c36,
};
