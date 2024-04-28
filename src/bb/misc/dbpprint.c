#include "PR/os_internal.h"
#include "PR/bcp.h"

void __osBbPPutter(void* str, char* buf, size_t n) {
    int i = 0;

    while (n != 0) {
        RDB_WRITE16(RDB_E0400_REG, 0x500 | buf[i]);
        IO_READ(RDB_20000_REG);

        RDB_WRITE16(RDB_E8000_REG, 1);
        IO_READ(RDB_20000_REG);

        i++;

        while (!(RDB_READ16(RDB_E8000_REG) & 8)) {
            ;
        }

        RDB_WRITE16(RDB_E8000_REG, 8);
        IO_READ(RDB_20000_REG);

        n--;
    }
}
