#include <string.h>

void* memmove(void* dst, const void* src, size_t size) {
	unsigned char* d = (unsigned char*) dst;
	const unsigned char* s = (const unsigned char*) src;

	if(dst < src) {
		for(size_t i = 0; i < size; i++)
			d[i] = s[i];
	} else {
		for(size_t i = size; i != 0; i--)
			d[i-1] = s[i-1];
	}

	return dst;
}