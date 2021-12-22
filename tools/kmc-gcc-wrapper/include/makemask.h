// MAKEMASK.OUT
#define BIN_FILE "kmc/gcc/MIPSE/BIN/MAKEMASK.OUT"
uint8_t dummy;
uintptr_t loadAddr = 0x01080000;
uintptr_t startAddr = 0x01080864;
int fileOffset = 0x1000;
int codeDataLength = 0x506c + 0x1ef8;
size_t bssSize = 0x01086f64;
uintptr_t mallocAddr = 0x01081b60;
uintptr_t reallocAddr = 0x01081da8;
uintptr_t environAddr = 0x010871bc;
uintptr_t _unix2dosnameAddr = 0x010822d4;
uintptr_t nops[] = {
    (uintptr_t)&dummy,
};

uintptr_t int21Addrs[] = {
    0x01080b98,
    0x01080ba5,
    0x01080bb8,
    0x01080bd6,
    0x01080be7,
    0x01080bff,
    0x01080c17,
    0x01080c36,
    0x01080c53,
    0x01080c6a,
    0x01080c83,
    0x01080c92,
    0x01080ca9,
    0x01080cbf,
    0x01080cdf,
    0x01080d0b,
    0x01080d3a,
    0x01080d4e,
    0x01080d62,
    0x01080d76,
    0x01080d7e,
    0x01080d97,
    0x01080daf,
    0x01080dbe,
    0x01080dd2,
    0x01080df3,
    0x01080e02,
    0x01080e1f,
    0x01080e37,
    0x01080e79,
    0x01080ea3,
    0x01080ece,
};
