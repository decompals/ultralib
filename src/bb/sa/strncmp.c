#include "PR/os_internal.h"

int strncmp(char* s, char* t, int n) {
    int i;

    for (i = 0; (*s == *t) && (i < n); i++) {
        if (*s == 0) {
            if (*t == 0) {
                return 0;
            }
            break;
        }
        if (*t == 0) {
            break;
        }
        s++;
        t++;
    }
    return (i != n) ? *s - *t : 0;
}
