#include "PRinternal/macros.h"
#include "PR/os_internal.h"
#include "PRinternal/controller.h"

#if BUILD_VERSION < VERSION_I

s32 osPfsReSizeFile(OSPfs* pfs, u16 company_code, u32 game_code, u8* game_name, u8* ext_name, int length) {
    int startpage;
    s32 ret = 0;
    s32 newsizeinpages;
    __OSInode inode;
    __OSInode previnode;
    __OSDir dir;
    s32 file_no;
    s32 oldsizeinpages;
    __OSInodeUnit firstpage;
    __OSInodeUnit page;
    u16 sum = 0;
    s32 bytes_not_used;
    s32 remaining;
    u8 bank;
    int decleared;
    s32 numbanksprocessed = 0;
    s32 numbanksadded = 0;
    s32 i;
    __OSInodeUnit lastpage;
    int finalpage;
    s32 prevbankfinalpage = 0;
    u8 prevbank = 0;
    u8 origlastbank = 0;
    s32 numbankchanges = 0;
    __OSInodeUnit retainedpage;
    s32 minpage;

    if (company_code == 0 || game_code == 0) {
        return PFS_ERR_INVALID;
    }

    newsizeinpages = (length + (PFS_ONE_PAGE * BLOCKSIZE - 1)) / (PFS_ONE_PAGE * BLOCKSIZE);

    PFS_CHECK_STATUS();
    PFS_CHECK_ID();

    ret = osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, &file_no);

    if (ret != 0 && ret != PFS_ERR_INVALID) {
        return ret;
    }

    if (file_no == -1) {
        return PFS_ERR_INVALID;
    }

    if (newsizeinpages != 0) {
        SET_ACTIVEBANK_TO_ZERO();

        ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir));

        if (dir.start_page.ipage < pfs->inode_start_page) {
            return PFS_ERR_INCONSISTENT;
        }

#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
        startpage = dir.start_page.inode_t.page;
        prevbankfinalpage = startpage;
        oldsizeinpages = 0;
#else
        oldsizeinpages = 0;
        prevbankfinalpage = startpage = dir.start_page.inode_t.page;
#endif

        for (bank = dir.start_page.inode_t.bank; bank < pfs->banks;) {
            ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));

            page = inode.inode_page[startpage];

            while (page.ipage >= pfs->inode_start_page) {
                oldsizeinpages++;
                prevbankfinalpage = page.inode_t.page;
                page = inode.inode_page[page.inode_t.page];

                if (page.inode_t.bank != bank) {
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
                    startpage = page.inode_t.page;
                    bank = page.inode_t.bank;
                    prevbankfinalpage = page.inode_t.page;
#else
                    bank = page.inode_t.bank;
                    prevbankfinalpage = startpage = page.inode_t.page;
#endif
                    break;
                }
            }

            if (page.ipage == PFS_EOF) {
                oldsizeinpages++;
                break;
            }
        }

        origlastbank = bank;

        if (bank >= pfs->banks) {
            return PFS_ERR_INCONSISTENT;
        }

        if (newsizeinpages > oldsizeinpages) {
            remaining = newsizeinpages - oldsizeinpages;

            ret = osPfsFreeBlocks(pfs, &bytes_not_used);

            if (remaining > bytes_not_used / (PFS_ONE_PAGE * BLOCKSIZE)) {
                return PFS_DATA_FULL;
            }

            prevbank = bank;

            for (i = 0; i < ARRLEN(inode.inode_page); i++) {
                previnode.inode_page[i].ipage = inode.inode_page[i].ipage;
            }

            for (bank = 0; bank < pfs->banks; bank++) {
                ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
                ERRCK(__osPfsDeclearPage(pfs, &inode, remaining, &startpage, bank, &decleared, &finalpage));

                if (startpage != -1) {
                    if (numbanksprocessed != 0 || origlastbank != bank) {
                        numbanksadded++;
                        previnode.inode_page[prevbankfinalpage].inode_t.bank = bank;
                        previnode.inode_page[prevbankfinalpage].inode_t.page = startpage;
                        ERRCK(__osPfsRWInode(pfs, &previnode, PFS_WRITE, prevbank));
                    }

                    for (i = 0; i < ARRLEN(inode.inode_page); i++) {
                        previnode.inode_page[i].ipage = inode.inode_page[i].ipage;
                    }

                    if (remaining > decleared) {
                        remaining -= decleared;
                    } else {
                        remaining = 0;
                        prevbank = bank;
                        break;
                    }

                    prevbankfinalpage = finalpage;
                    prevbank = bank;
                    numbanksprocessed++;
                }
            }

            if (remaining > 0 || startpage == -1) {
                return PFS_ERR_INCONSISTENT;
            }

            if (numbanksadded == 0) {
                previnode.inode_page[prevbankfinalpage].inode_t.bank = bank;
                previnode.inode_page[prevbankfinalpage].inode_t.page = startpage;
            }

            ERRCK(__osPfsRWInode(pfs, &previnode, PFS_WRITE, prevbank));
        } else if (newsizeinpages < oldsizeinpages) {
            retainedpage = dir.start_page;

            for (bank = dir.start_page.inode_t.bank; bank < pfs->banks;) {
                ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));

                firstpage = retainedpage;
                page = inode.inode_page[retainedpage.inode_t.page];
                newsizeinpages--;
                minpage = bank > 0 ? 1 : pfs->inode_start_page;

                if (page.inode_t.page < minpage && page.ipage != PFS_EOF) {
                    return PFS_ERR_INCONSISTENT;
                }

                while (newsizeinpages > 0 && page.ipage >= pfs->inode_start_page) {
                    newsizeinpages--;
                    firstpage = page;
                    page = inode.inode_page[page.inode_t.page];

                    if (page.inode_t.bank != bank) {
                        if (newsizeinpages == 1) {
                            inode.inode_page[firstpage.inode_t.page].ipage = PFS_EOF;
                            ERRCK(__osPfsRWInode(pfs, &inode, PFS_WRITE, bank));
                        }

                        numbankchanges++;
                        retainedpage = page;
                        bank = page.inode_t.bank;
                        ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
                        break;
                    }
                }

                if (newsizeinpages == 0) {
                    break;
                }
            }

            if (bank >= pfs->banks) {
                return PFS_ERR_INCONSISTENT;
            }

            if (numbankchanges == 0) {
                inode.inode_page[firstpage.inode_t.page].ipage = PFS_EOF;
            } else {
                inode.inode_page[page.inode_t.page].ipage = PFS_EOF;
            }

            if (page.ipage == PFS_EOF) {
                inode.inode_page[firstpage.inode_t.page].ipage = PFS_PAGE_NOT_USED;
                ERRCK(__osPfsRWInode(pfs, &inode, PFS_WRITE, bank));
            } else {
                retainedpage = page;

                for (bank = page.inode_t.bank; bank < pfs->banks;) {
                    ERRCK(__osPfsReleasePages(pfs, &inode, retainedpage.inode_t.page, &sum, bank, &lastpage, FALSE));
                    ERRCK(__osPfsRWInode(pfs, &inode, PFS_WRITE, bank));

                    if (lastpage.ipage == PFS_EOF) {
                        break;
                    }

                    bank = lastpage.inode_t.bank;
                    retainedpage = lastpage;
                    ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
                }

                if (bank >= pfs->banks) {
                    return PFS_ERR_INCONSISTENT;
                }
            }

            if (dir.data_sum > sum) {
                dir.data_sum -= sum;
            } else {
                dir.data_sum = sum - dir.data_sum;
            }

            ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir, FALSE));
        }

        return ret;
    } else {
        return PFS_ERR_INVALID;
    }
}

#endif
