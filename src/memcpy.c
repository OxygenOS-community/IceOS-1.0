#include <string.h>

void *memcpy(void *dst, const void *src, u32 n) {

	u8 *bdst = (u8 *)dst;
	u8 *bsrc = (u8 *)src;

	for (int i = 0; i < n; i++) {
		bdst[i] = bsrc[i];
	}

	return dst;
}