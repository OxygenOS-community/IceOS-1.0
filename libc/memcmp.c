#include "string.h"

int memcmp(const char *str1, const char *str2, int count) {
    const char *s1 = (const char*)str1;
    const char *s2 = (const char*)str2;

    while (count-- > 0) {
       if (*s1++ != *s2++) return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}