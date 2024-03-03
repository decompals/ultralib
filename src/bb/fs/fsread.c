#include "PR/os_internal.h"
#include "PR/bbfs.h"

s32 osBbFRead(s32 fd, u32 off, void* buf, u32 len) {
    s32 rv;
    BbInode* in;
    u32 count;
    u32 b;
    u32 i;
    BbFat16* fat;

    if (fd >= (u32)BB_INODE16_NUM) {
        return -3;
    }

    rv = __osBbFsGetAccess();
    if (rv < 0) {
        return rv;
    }

    fat = __osBbFat;
    in = &fat->inode[fd];
    rv = -3;

    if ((in->type != 0) && !(off & 0x3FFF)) {
        if ((off < in->size) && (off + len >= off)) {
            if (in->size < off + len) {
                len = in->size - off;
            }

            if (len == 0) {
                rv = 0;
                goto end;
            }

            b = in->block;

            for (i = 0; i < off / 0x4000; i++) {
                b = fat[b >> 0xC].entry[b & 0xFFF];
            } 

            count = 0;

            while (len != 0) {
                if ((b == 0) || (b >= __osBbFsBlocks - 0x10)) {
                    goto end;
                }

                rv = osBbCardReadBlock(0, b, buf, NULL);
                if (rv < 0) {
                    goto end;
                }

                b = fat[b >> 0xC].entry[b & 0xFFF];
                buf += 0x4000;
                len = (len > 0x4000) ? (len - 0x4000) : 0;
                count += 0x4000;
            }
            rv = count;
        }
    }

end:
    __osBbFsRelAccess();
    return rv;
}
