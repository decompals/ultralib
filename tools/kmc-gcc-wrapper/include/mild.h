// MILD.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/MILD.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x01080000;
uintptr_t startAddr = 0x010813b8;
int fileOffset = 0x1000;
int codeDataLength = 0x155ec + 0x1ee68;
size_t bssSize = 0x010b4454;
uintptr_t mallocAddr = 0x0108fda4;
uintptr_t reallocAddr = 0x0108ffec;
uintptr_t environAddr = 0x010b49bc;
uintptr_t _unix2dosnameAddr = 0x01090518;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x0108ead9,
    0x0108eaec,
    0x0108eb0a,
    0x0108eb1b,
    0x0108eb33,
    0x0108eb4b,
    0x0108eb6a,
    0x0108eb87,
    0x0108eb9e,
    0x0108ebb7,
    0x0108ebc6,
    0x0108ebdd,
    0x0108ebf3,
    0x0108ec13,
    0x0108ec3f,
    0x0108ec6e,
    0x0108ec82,
    0x0108ec96,
    0x0108ecaa,
    0x0108ecb2,
    0x0108eccb,
    0x0108ece3,
    0x0108ecf2,
    0x0108ed06,
    0x0108ed27,
    0x0108ed36,
    0x0108ed53,
    0x0108ed6b,
    0x0108edad,
    0x0108edd7,
    0x0108ee02,
};

#define REDIRECT_SPAWNVPE
uintptr_t spawnvpeAddr = 0x01093208;
