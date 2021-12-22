// CPP.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/CPP.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x1080000;
uintptr_t startAddr = 0x010811d4;
int fileOffset = 0x1000;
int codeDataLength = 0x15520 + 0x12b8;
size_t bssSize = 0x010967d8;
uintptr_t mallocAddr = 0x010916d4;
uintptr_t reallocAddr = 0x0109191c;
uintptr_t environAddr = 0x0109cca0;
uintptr_t _unix2dosnameAddr = 0x01091e48;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x010903fc,
    0x01090409,
    0x0109041c,
    0x0109043a,
    0x0109044b,
    0x01090463,
    0x0109047b,
    0x0109049a,
    0x010904b7,
    0x010904ce,
    0x010904e7,
    0x010904f6,
    0x0109050d,
    0x01090523,
    0x01090543,
    0x0109056f,
    0x0109059e,
    0x010905b2,
    0x010905c6,
    0x010905da,
    0x010905e2,
    0x010905fb,
    0x01090613,
    0x01090622,
    0x01090636,
    0x01090657,
    0x01090666,
    0x01090683,
    0x0109069b,
    0x010906dd,
    0x01090707,
    0x01090732,
};
