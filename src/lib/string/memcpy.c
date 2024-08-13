#include "string.h"

void* memcpy(void* restrict dst, const void* restrict src, size_t n){
    unsigned char* ucdst = (unsigned char*)dst;
    unsigned char* ucsrc = (unsigned char*)src;
    for(size_t i = 0; i < n; i++)
        ucdst[i] = ucsrc[i];
    return dst;
}
