#include "stdlib.h"
#include "string.h"
#include "xstdio.h"

// TODO: these come from headers
#ident "$Revision: 1.23 $"
#ident "$Revision: 1.34 $"
#ident "$Revision: 1.5 $"

#define BUFF_LEN 0x20

static short _Ldunscale(short *, printf_struct *);
static void _Genld(printf_struct *, char, char *, short, short);

static const double pows[] = {10e0L, 10e1L, 10e3L, 10e7L, 10e15L, 10e31L, 10e63L, 10e127L, 10e255L};

// float properties
#define _D0 0
#define _DBIAS 0x3ff
#define _DLONG 1
#define _DOFF 4
#define _FBIAS 0x7e
#define _FOFF 7
#define _FRND 1
#define _LBIAS 0x3ffe
#define _LOFF 15
// integer properties
#define _C2 1
#define _CSIGN 1
#define _ILONG 0
#define _MBMAX 8
#define NAN 2
#define INF 1
#define FINITE -1
#define _DFRAC ((1 << _DOFF) - 1)
#define _DMASK (0x7fff & ~_DFRAC)
#define _DMAX ((1 << (15 - _DOFF)) - 1)
#define _DNAN (0x8000 | _DMAX << _DOFF | 1 << (_DOFF - 1))
#define _DSIGN 0x8000
#define _D1 1 // big-endian order
#define _D2 2
#define _D3 3

void _Ldtob(printf_struct *args, char type) {
    char buff[BUFF_LEN];
    char *p;
    f64 ldval;
    short err;
    short nsig;
    short exp;

    // char unused[0x4];
    p = buff;
    ldval = args->value.f64;

    if (args->precision < 0) {
        args->precision = 6;
    } else {
        if (args->precision == 0 && (type == 'g' || type == 'G')) {
            args->precision = 1;
        }
    }

    err = _Ldunscale(&exp, args);
    
    if (err > 0) {
        memcpy(args->buff, err == 2 ? "NaN" : "Inf", args->part2_len = 3);
        return;
    }
    
    if (err == 0) {
        nsig = 0;
        exp = 0;
    } else {
        int i;
        int n;

        if (ldval < 0) {
            ldval = -ldval;
        }

        exp = exp * 30103 / 100000 - 4;
        
        if (exp < 0) {
            n = ((-exp + 3) & ~3);
            exp = -n;

            for (i = 0; n > 0; n >>= 1, i++) {
                if ((n & 1) != 0) {
                    ldval *= pows[i];
                }
            }
        } else if (exp > 0) {
            f64 factor = 1;
            
            exp &= ~3;
            
            for (n = exp, i = 0; n > 0; n >>= 1, i++) {
                if ((n & 1) != 0) {
                    factor *= pows[i];
                }
            }

            ldval /= factor;
        }
        {
            int gen = args->precision + ((type == 'f') ? 10 + exp : 6);
            
            if (gen > 0x13) {
                gen = 0x13;
            }
            
            *p++ = '0';

            while (gen > 0 && 0 < ldval) {
                int j;
                int lo = ldval;
                
                if ((gen -= 8) > 0) {
                    ldval = (ldval - lo) * 1.0e8;
                }
                
                p = p + 8;
                
                for (j = 8; lo > 0 && --j >= 0;) {
                    ldiv_t qr = ldiv(lo, 10);
                    *--p = qr.rem + '0';
                    lo = qr.quot;
                }
                
                while (--j >= 0) {
                    p--;
                    *p = '0';
                }
                p += 8;
            }

            gen = p - &buff[1];

            for (p = &buff[1], exp += 7; *p == '0'; p++) {
                --gen, --exp;
            }

            nsig = args->precision + ((type == 'f') ? exp + 1 : ((type == 'e' || type == 'E') ? 1 : 0));
            
            if (gen < nsig) {
                nsig = gen;
            }

            if (nsig > 0) {
                char drop;
                int n;

                if (nsig < gen && p[nsig] > '4')
                {
                    drop = '9';
                } else {
                    drop = '0';
                }

                for (n = nsig; p[--n] == drop;) {
                    nsig--;
                }

                if (drop == '9') {
                    p[n]++;
                }
                
                if (n < 0) {
                    --p, ++nsig, ++exp;
                }
            }
        }
    }
    _Genld(args, type, p, nsig, exp);
}

// _Ldunscale
// _Genld
