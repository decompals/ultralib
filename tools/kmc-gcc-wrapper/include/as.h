// AS.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/AS.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x1080000;
uintptr_t startAddr = 0x01093cd4;
int fileOffset = 0x1000;
int codeDataLength = 0x5beb4 + 0x1a8c;
size_t bssSize = 0x010dd940;
uintptr_t mallocAddr = 0x010d77cc;
uintptr_t reallocAddr = 0x010d7a14;
uintptr_t environAddr = 0x010e6340;
uintptr_t getenvAddr = 0x010d9ad8; // Not used
uintptr_t _unix2dosnameAddr = 0x010d7f40;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x010d6b78,
    0x010d6b85,
    0x010d6b98,
    0x010d6bb6,
    0x010d6bc7,
    0x010d6bdf,
    0x010d6bf7,
    0x010d6c16,
    0x010d6c33,
    0x010d6c4a,
    0x010d6c63,
    0x010d6c72,
    0x010d6c89,
    0x010d6c9f,
    0x010d6cbf,
    0x010d6ceb,
    0x010d6d1a,
    0x010d6d2e,
    0x010d6d42,
    0x010d6d56,
    0x010d6d5e,
    0x010d6d77,
    0x010d6d8f,
    0x010d6d9e,
    0x010d6db2,
    0x010d6dd3,
    0x010d6de2,
    0x010d6dff,
    0x010d6e17,
    0x010d6e59,
    0x010d6e83,
    0x010d6eae,
};

// Extra stuff for AS specifically
#define IS_AS
uintptr_t dos_ext = 0x010e62e0;
uintptr_t _kmc_prg_no = 0x010e685c;

