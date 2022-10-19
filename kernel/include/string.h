#ifndef _STRING_H
#define _STRING_H

size_t  strlen(const char*);
int		memcmp(const void*, const void*);
void*	memcpy(void* dst, const void* src, size_t n);
void*	memset(void* str, int c, size_t n);

#endif
