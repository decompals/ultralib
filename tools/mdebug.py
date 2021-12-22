#!/usr/bin/env python3
#
#   .mdebug section
#

"""
References:
https://www.cs.unibo.it/~solmi/teaching/arch_2002-2003/AssemblyLanguageProgDoc.pdf
https://web.archive.org/web/20010628021622/http://reality.sgi.com/davea/objectinfo.html
https://github.com/astrelsky/ghidra_mdebug
https://github.com/Rozelette/print-mdebug/blob/master/print_mdebug.py
https://opensource.apple.com/source/gcc_legacy/gcc_legacy-938/gcc/mips-tdump.c.auto.html
https://github.com/uhhpctools/openuh/blob/master/osprey-gcc-4.2.0/gcc/mips-tdump.c
https://github.com/bminor/binutils-gdb/blob/master/gdb/mdebugread.c

(stabs docs):
https://sourceware.org/gdb/current/onlinedocs/stabs.html

(ecoff docs):
https://web.archive.org/web/20160305114748/http://h41361.www4.hp.com/docs/base_doc/DOCUMENTATION/V50A_ACRO_SUP/OBJSPEC.PDF
https://chromium.googlesource.com/native_client/nacl-toolchain/+/refs/tags/gcc-4.4.3/binutils/gas/ecoff.c
https://kernel.googlesource.com/pub/scm/linux/kernel/git/hjl/binutils/+/hjl/secondary/include/coff/symconst.h
"""

from enum import IntEnum
import struct

class EcoffBasicType(IntEnum):
    NIL         =  0 # 
    ADR         =  1 # pointer-sized integer type
    CHAR        =  2 # char
    UCHAR       =  3 # unsigned char
    SHORT       =  4 # short
    USHORT      =  5 # unsigned short
    INT         =  6 # int
    UINT        =  7 # unsigned int
    LONG        =  8 # long
    ULONG       =  9 # unsigned long
    FLOAT       = 10 # float
    DOUBLE      = 11 # double
    STRUCT      = 12 # struct
    UNION       = 13 # union
    ENUM        = 14 # enum
    TYPEDEF     = 15 # type definition
    RANGE       = 16 # subrange of int
    SET         = 17 # pascal set
    COMPLEX     = 18 # FORTRAN complex
    DCOMPLEX    = 19 # FORTRAN double com[plex
    INDIRECT    = 20 # forward or unnamed typedef
    FIXEDDEC    = 21 # Fixed point decimal
    FLOATDEC    = 22 # Floating point decimal
    STRING      = 23 # Varying length character string
    BIT         = 24 # Aligned bit tring
    PICTURE     = 25 # picture
    VOID        = 26 # void
    LONGLONG    = 27 # long long int
    ULONGLONG   = 28 # unsigned long long int
    LONG64      = 30 # 
    ULONG64     = 31 # 
    LONGLONG64  = 32 # 
    ULONGLONG64 = 33 # 
    ADR64       = 34 # 
    INT64       = 35 # 
    UINT64      = 36 # 
    MAX         = 64 # 

class EcoffSc(IntEnum):
    NIL         =  0
    TEXT        =  1 # .text symbol
    DATA        =  2 # .data symbol
    BSS         =  3 # .bss symbol
    REGISTER    =  4 # value of symbol is register number
    ABS         =  5 # value of symbol is absolute
    UNDEFINED   =  6 # value of symbol is undefined
    CDBLOCAL    =  7 # variable value is in se->va.??
    BITS        =  8 # variable is a bit field
    CDBSYSTEM   =  9 # variable value is in cdb address space
    REGIMAGE    = 10 # register value is saved on stack
    INFO        = 11 # symbol contains debugger information
    USERSTRUCT  = 12 # address in struct user for current process
    SDATA       = 13 # load time only small data
    SBSS        = 14 # load time only small common
    RDATA       = 15 # load time only read-only data
    VAR         = 16 # var parameter (FORTRAN, Pascal)
    COMMON      = 17 # common variable
    SCOMMON     = 18 # small common
    VARREGISTER = 19 # var parameter in a register
    VARIANT     = 20 # variant record
    SUNDEFINED  = 21 # small undefined (external) data
    INIT        = 22 # .init section symbol
    BASEDVAR    = 23 # FORTRAN or PL/1 ptr based var
    XDATA       = 24 # exception handling data
    PDATA       = 25 # procedure section
    FINI        = 26 # .fini section
    RCONST      = 27 # .rconst section
    MAX         = 32 # 

class EcoffSt(IntEnum):
    NIL        =  0 # 
    GLOBAL     =  1 # external symbol
    STATIC     =  2 # static symbol
    PARAM      =  3 # procedure argument
    LOCAL      =  4 # local variable
    LABEL      =  5 # label
    PROC       =  6 # procedure
    BLOCK      =  7 # beginning of block
    END        =  8 # end of something
    MEMBER     =  9 # member of struct/union/enum/..
    TYPEDEF    = 10 # type definition
    FILE       = 11 # filename
    REGRELOC   = 12 # register relocation
    FORWARD    = 13 # forwarding address
    STATICPROC = 14 # load time only static procedures
    # (CONSTANT and STAPARAM are in different orders between different sources...)
    CONSTANT   = 15 # constant
    STAPARAM   = 16 # FORTRAN static parameters
    STRUCT     = 26 # structure
    UNION      = 27 # union
    ENUM       = 28 # enum
    INDIRECT   = 34 # 

class EcoffTypeQualifier(IntEnum):
    NIL   = 0 # 
    PTR   = 1 # pointer
    PROC  = 2 # procedure
    ARRAY = 3 # array
    FAR   = 4 # longer addressing
    VOL   = 5 # volatile
    CONST = 6 # constant
    MAX   = 8 # 

class EcoffLanguageCode(IntEnum):
    C           = 0
    PASCAL      = 1
    FORTRAN     = 2
    ASM         = 3
    MACHINE     = 4
    NIL         = 5 
    ADA         = 6
    PL1         = 7
    COBOL       = 8
    STDC        = 9
    CPLUSPLUSV2 = 10
    MAX         = 11

def get_bitrange(value, start, length):
    return (value >> start) & ((1 << length) - 1)

def sign_extend_16(value):
    return (value & 0x7FFF) - (value & 0x8000)

def sign_extend_4(value):
    return (value & 0x7) - (value & 0x8)

class EcoffLiner:
    """
    ECOFF Line Numbers Mapping

    typedef struct sLINER {
        s32 count : 4;
        s32 delta : 4;
    } tLINER, *pLINER;
    """

    def __init__(self, data) -> None:
        self.count = get_bitrange(data[0], 0, 4) + 1
        self.delta = get_bitrange(data[0], 4, 4)

        if self.delta == 8:
            self.is_extended = True
            self.delta = sign_extend_16((data[1] << 8) | data[2])
            self.data = data[:3]
        else:
            self.is_extended = False
            self.delta = sign_extend_4(self.delta)
            self.data = data[:1]

    def __str__(self) -> str:
        return f"""= EcoffLiner =============
delta        = {self.delta}
count        = {self.count}
extended     = {self.is_extended}"""

class EcoffSymr:
    """
    ECOFF Local Symbol

    typedef struct sSymr {
        s32 iss;            /* index into String Space of name */
        s32 value;          /* symbol value */
        EcoffSt st    : 6;  /* symbol type */
        EcoffSc sc    : 5;  /* storage class - text, data, etc */
        s32 _reserved : 1;  /* reserved bit */
        s32 index     : 20; /* index into sym/aux table */
    } tSymr, *pSymr; // size = 0xC
    """
    SIZE = 0xC

    def __init__(self, parent, data):
        self.parent = parent # can be either Fdr or Pdr
        self.pdr = parent if type(parent) == EcoffPdr else None
        self.fdr = self.pdr.parent if self.pdr is not None else parent

        self.data = data[:EcoffSymr.SIZE]

        self.iss, self.value, bits = struct.unpack(">III", self.data)
        self.st = EcoffSt(get_bitrange(bits, 26, 6))
        self.sc = EcoffSc(get_bitrange(bits, 21, 5))
        self._reserved = get_bitrange(bits, 20, 1)
        self.index = get_bitrange(bits, 0, 20)

        self.name = self.fdr.read_string(self.iss)

        assert self._reserved == 0 # Sanity check

    def __str__(self) -> str:
        return f"""= EcoffSymr ==============
iss       = 0x{self.iss:08X}
value     = 0x{self.value:08X}
st        = st{self.st.name}
sc        = sc{self.sc.name}
_reserved = {self._reserved}
index     = 0x{self.index:05X}
name      = {self.name}"""

class EcoffPdr:
    """
    ECOFF Procedure Descriptor

    typedef struct sPDR {
        s32 addr;           /* memory address of start of procedure */
        s32 isym;           /* start of local symbol entries */
        s32 iline;          /* start of line number entries */
        s32 regmask;        /* save register mask */
        s32 regoffset;      /* save register offset */
        s32 iopt;           /* start of optimization symbol entries */
        s32 fregmask;       /* save floating point register mask */
        s32 fregoffset;     /* save floating point register offset */
        s32 frameoffset;    /* frame size */
        u16 framereg;       /* frame pointer register */
        u16 pcreg;          /* offset or reg of return pc */
        s32 lnLow;          /* lowest line in the procedure */
        s32 lnHigh;         /* highest line in the procedure */
        s32 cbLineOffset;   /* byte offset for this procedure from the fd base */
#ifdef 64_BIT
        // TODO there's a bitfield in here
        s32 gpPrologue;     /* byte size of GP prologue */
        s32 gpUsed;         /* true if the procedure uses GP */
        s32 regFrame;       /* true if register frame procedure */
        s32 prof;           /* true if compiled with -pg */
        s32 localOffset;    /* offset of local variables from vfp */
#endif
    } tPDR, *pPDR; // size = 0x34
    """
    SIZE = 0x34

    def __init__(self, fdr, data) -> None:
        self.parent = fdr
        self.data = data[:EcoffPdr.SIZE]

        self.addr, self.isym, self.iline, self.regmask, \
        self.regoffset, self.iopt, self.fregmask, self.fregoffset, \
        self.frameoffset, self.framereg, self.pcreg, self.lnLow, \
        self.lnHigh, self.cbLineOffset = struct.unpack(">IIIIIIIIIHHIII", self.data)

        self.symrs = []

        i = self.isym
        symr = self.parent.symrs[i]
        assert symr.st == EcoffSt.PROC or symr.st == EcoffSt.STATICPROC
        # Inherit procedure name from procedure symbol
        self.name = symr.name

        self.symrs.append(symr)
        while not (symr.st == EcoffSt.END and symr.name == self.symrs[0].name):
            i += 1
            symr = self.parent.symrs[i]
            self.symrs.append(symr)

        assert symr.st == EcoffSt.END and symr.sc == EcoffSc.TEXT
        self.size = symr.value # value field of an stEND and scTEXT symbol is the procedure size
        assert self.size % 4 == 0

        # indexed by asm word offset from proc start
        self.lines = []

        # ilineMax = self.parent.parent.hdrr.ilineMax
        # cbLine = self.parent.parent.hdrr.cbLine
        # cbLineOffset = self.parent.parent.hdrr.cbLineOffset
        # ilineBase = self.parent.ilineBase
        # cline = self.parent.cline
        # cbLineOffset = self.parent.cbLineOffset
        # cbLine = self.parent.cbLine
        # lnLow = self.lnLow
        # lnHigh = self.lnHigh
        # iline = self.iline

        elf_data = self.parent.parent.parent.data

        line_no = self.lnLow # first line in the procedure
        line_data = self.parent.parent.hdrr.cbLineOffset + self.parent.cbLineOffset + self.cbLineOffset
        # line_end = self.parent.parent.hdrr.cbLineOffset + self.parent.cbLineOffset + self.parent.cbLine

        # print(self)
        # print(f"{self.name} [{self.lnLow}:{self.lnHigh}]")
        # print(self.size//4)
        while len(self.lines) < self.size//4:
            # assert line_data < line_end , "Overflow in line numbers table"

            liner = EcoffLiner(elf_data[line_data:])
            line_no += liner.delta
            # if line_no < self.lnLow or line_no > self.lnHigh:
            #     break

            # print(liner)
            for i in range(liner.count):
                # print(f"[{len(self.lines)}] {line_no}")
                self.lines.append(line_no)

            line_data += len(liner.data)

    def __str__(self) -> str:
        return f"""= EcoffPdr ===============
addr         = 0x{self.addr:08X}
isym         = 0x{self.isym:08X}
iline        = 0x{self.iline:08X}
regmask      = 0b{self.regmask:032b}
regoffset    = 0x{self.regoffset:08X}
iopt         = 0x{self.iopt:08X}
fregmask     = 0b{self.fregmask:032b}
fregoffset   = 0x{self.fregoffset:08X}
frameoffset  = 0x{self.frameoffset:08X}
framereg     = ${self.framereg}
pcreg        = ${self.pcreg}
lnLow        = {self.lnLow}
lnHigh       = {self.lnHigh}
cbLineOffset = 0x{self.cbLineOffset:08X}
name         = {self.name}"""

class EcoffFdr:
    """
    ECOFF File Descriptor

    typedef struct sFDR {
        u32 adr;                        /* memory address of beginning of file */
        s32 rss;                        /* file name (of source, if known) */
        s32 issBase;                    /* file's string space */
        s32 cbSs;                       /* number of bytes in the ss */
        s32 isymBase;                   /* beginning of symbols */
        s32 csym;                       /* count file's of symbols */
        s32 ilineBase;                  /* file's line symbols */
        s32 cline;                      /* count of file's line symbols */
        s32 ioptBase;                   /* file's optimization entries */
        s32 copt;                       /* count of file's optimization entries */
        u16 ipdFirst;                   /* start of procedures for this file */
        u16 cpd;                        /* count of procedures for this file */
        s32 iauxBase;                   /* file's auxiliary entries */
        s32 caux;                       /* count of file's auxiliary entries */
        s32 rfdBase;                    /* index into the file indirect table */
        s32 crfd;                       /* count file indirect entries */
        EcoffLanguageCode lang : 5;     /* language for this file */
        u32 fMerge             : 1;     /* whether this file can be merged */
        u32 fReadin            : 1;     /* true if it was read in (not just created) */
        u32 fBigEndian         : 1;     /* true if AUXU's are big endian */
        u32 glevel             : 2;     /* level this file was compiled with */
        u32 _reserved          : 20;    /* reserved bits */
        s32 cbLineOffset;               /* byte offset from header for this file ln's */
        s32 cbLine;                     /* size of lines for this file */
    } tFDR, *pFDR; // size = 0x48
    """
    SIZE = 0x48

    @staticmethod
    def from_binary(mdebug, i):
        # Init
        if 'init' not in EcoffFdr.from_binary.__dict__:
            EcoffFdr.from_binary.cache = {}
            EcoffFdr.from_binary.init = True
        # Parent Init
        if mdebug not in EcoffFdr.from_binary.cache:
            EcoffFdr.from_binary.cache[mdebug] = {}
        # Cache hit
        if i in EcoffFdr.from_binary.cache[mdebug]:
            return EcoffFdr.from_binary.cache[mdebug][i]
        # Cache miss
        cbFdOffset = mdebug.hdrr.cbFdOffset
        elf_data = mdebug.parent.data
        EcoffFdr.from_binary.cache[mdebug][i] = EcoffFdr(mdebug, elf_data[cbFdOffset+i*EcoffFdr.SIZE:cbFdOffset+(i+1)*EcoffFdr.SIZE])
        return EcoffFdr.from_binary.cache[mdebug][i]

    def __init__(self, mdebug, data) -> None:
        self.parent = mdebug
        self.data = data[:EcoffFdr.SIZE]

        self.adr, self.rss, self.issBase, self.cbSs, \
            self.isymBase, self.csym, self.ilineBase, self.cline, \
            self.ioptBase, self.copt, self.ipdFirst, self.cpd, \
            self.iauxBase, self.caux, self.rfdBase, self.crfd, \
            bits, self.cbLineOffset, self.cbLine = struct.unpack(">IIIIIIIIIIHHIIIIIII", self.data)

        self.lang = EcoffLanguageCode(get_bitrange(bits, 27, 5))
        self.fMerge = get_bitrange(bits, 26, 1)
        self.fReadin = get_bitrange(bits, 25, 1)
        self.fBigEndian = get_bitrange(bits, 24, 1)
        self.glevel = get_bitrange(bits, 22, 2)
        self._reserved = get_bitrange(bits, 2, 20)

        self.name = self.parent.read_string(self.issBase + self.rss)

        # print(self)

        hdrr = self.parent.hdrr
        elf_data = self.parent.parent.data

        # Symbols
        self.symrs = []
        for i in range(self.csym):
            i += self.isymBase
            assert i < hdrr.isymMax , "Out of bounds in Local Symbol Table"
            symr = EcoffSymr(self, elf_data[hdrr.cbSymOffset+i*EcoffSymr.SIZE:hdrr.cbSymOffset+(i+1)*EcoffSymr.SIZE])
            self.symrs.append(symr)

        # PDRs
        self.pdrs = []
        for i in range(self.cpd):
            i += self.ipdFirst
            assert i < hdrr.ipdMax , "Out of bounds in Procedure Descriptor Table"
            pdr = EcoffPdr(self, elf_data[hdrr.cbPdOffset+i*EcoffPdr.SIZE:hdrr.cbPdOffset+(i+1)*EcoffPdr.SIZE])
            self.pdrs.append(pdr)

        self.size = sum([pdr.size for pdr in self.pdrs])

    def pdr_forname(self, procedure_name):
        for pdr in self.pdrs:
            if pdr.name == procedure_name:
                return pdr
        return None

    def pdr_foranyaddr(self, addr):
        for pdr in self.pdrs:
            if pdr.addr <= addr and pdr.addr + pdr.size > addr:
                return pdr
        return None

    def pdr_foraddr(self, addr):
        for pdr in self.pdrs:
            if pdr.addr == addr:
                return pdr
        return None

    def read_string(self, index):
        return self.parent.read_string(self.issBase + index)

    def __str__(self) -> str:
        return f"""= EcoffFdr ===============
adr          = 0x{self.adr:08X}
rss          = 0x{self.rss:08X}
issBase      = 0x{self.issBase:08X}
cbSs         = 0x{self.cbSs:08X}
isymBase     = 0x{self.isymBase:08X}
csym         = 0x{self.csym:08X}
ilineBase    = 0x{self.ilineBase:08X}
cline        = 0x{self.cline:08X}
ioptBase     = 0x{self.ioptBase:08X}
copt         = 0x{self.copt:08X}
ipdFirst     = 0x{self.ipdFirst:08X}
cpd          = 0x{self.cpd:08X}
iauxBase     = 0x{self.iauxBase:08X}
caux         = 0x{self.caux:08X}
rfdBase      = 0x{self.rfdBase:08X}
crfd         = 0x{self.crfd:08X}
lang         = {self.lang.name}
fMerge       = {bool(self.fMerge)}
fReadin      = {bool(self.fReadin)}
fBigEndian   = {bool(self.fBigEndian)}
glevel       = {self.glevel}
_reserved    = {self._reserved}
cbLineOffset = 0x{self.cbLineOffset:08X}
cbLine       = 0x{self.cbLine:08X}
name         = {self.name}"""

class EcoffHDRR:
    """
    Symbolic Header

    typedef struct sHDRR {
	    u16 magic;          /* 0x7009 */
	    u16 vstamp;         /* version stamp */
	    s32 ilineMax;       /* number of line number entries */
	    s32 cbLine;         /* number of bytes for line number entries */
	    s32 cbLineOffset;   /* offset to start of line number entries */
	    s32 idnMax;         /* max index into dense number table */
	    s32 cbDnOffset;     /* offset to start dense number table */
	    s32 ipdMax;         /* number of procedures */
	    s32 cbPdOffset;     /* offset to procedure descriptor table */
	    s32 isymMax;        /* number of local symbols */
	    s32 cbSymOffset;    /* offset to start of local symbols */
	    s32 ioptMax;        /* max index into optimization symbol entries */
	    s32 cbOptOffset;    /* offset to optimization symbol entries */
	    s32 iauxMax;        /* number of auxillary symbol entries */
	    s32 cbAuxOffset;    /* offset to start of auxillary symbol entries */
	    s32 issMax;         /* max index into local strings */
	    s32 cbSsOffset;     /* offset to start of local strings */
	    s32 issExtMax;      /* max index into external strings */
	    s32 cbSsExtOffset;  /* offset to start of external strings */
	    s32 ifdMax;         /* number of file descriptor entries */
	    s32 cbFdOffset;     /* offset to file descriptor table */
	    s32 crfd;           /* number of relative file descriptor entries */
	    s32 cbRfdOffset;    /* offset to relative file descriptor table */
	    s32 iextMax;        /* max index into external symbols */
	    s32 cbExtOffset;    /* offset to start of external symbol entries */
    } tHDRR, *pHDRR; // size = 0x60
    """
    HDRR_MAGIC = 0x7009
    SIZE = 0x60

    def __init__(self, data) -> None:
        self.data = data[:EcoffHDRR.SIZE]

        self.magic, self.vstamp, self.ilineMax, self.cbLine, \
            self.cbLineOffset, self.idnMax, self.cbDnOffset, self.ipdMax, \
            self.cbPdOffset, self.isymMax, self.cbSymOffset, self.ioptMax, \
            self.cbOptOffset, self.iauxMax, self.cbAuxOffset, self.issMax, \
            self.cbSsOffset, self.issExtMax, self.cbSsExtOffset, self.ifdMax, \
            self.cbFdOffset, self.crfd, self.cbRfdOffset, self.iextMax, \
            self.cbExtOffset = struct.unpack(">HHIIIIIIIIIIIIIIIIIIIIIII", self.data)
        
        assert self.magic == EcoffHDRR.HDRR_MAGIC , f"Symbolic Header magic value is incorrect. Got 0x{self.magic:04X}, expected 0x{EcoffHDRR.HDRR_MAGIC:04X}"

    def __str__(self) -> str:
        return f"""= EcoffHDRR ==============
magic         = 0x{self.magic:04X}
vstamp        = 0x{self.vstamp:04X}
ilineMax      = 0x{self.ilineMax:08X}
cbLine        = 0x{self.cbLine:08X}
cbLineOffset  = 0x{self.cbLineOffset:08X}
idnMax        = 0x{self.idnMax:08X}
cbDnOffset    = 0x{self.cbDnOffset:08X}
ipdMax        = 0x{self.ipdMax:08X}
cbPdOffset    = 0x{self.cbPdOffset:08X}
isymMax       = 0x{self.isymMax:08X}
cbSymOffset   = 0x{self.cbSymOffset:08X}
ioptMax       = 0x{self.ioptMax:08X}
cbOptOffset   = 0x{self.cbOptOffset:08X}
iauxMax       = 0x{self.iauxMax:08X}
cbAuxOffset   = 0x{self.cbAuxOffset:08X}
issMax        = 0x{self.issMax:08X}
cbSsOffset    = 0x{self.cbSsOffset:08X}
issExtMax     = 0x{self.issExtMax:08X}
cbSsExtOffset = 0x{self.cbSsExtOffset:08X}
ifdMax        = 0x{self.ifdMax:08X}
cbFdOffset    = 0x{self.cbFdOffset:08X}
crfd          = 0x{self.crfd:08X}
cbRfdOffset   = 0x{self.cbRfdOffset:08X}
iextMax       = 0x{self.iextMax:08X}
cbExtOffset   = 0x{self.cbExtOffset:08X}"""
