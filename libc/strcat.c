#include "string.h"

char* strcat(char* dst, const char* src) {
	char* ptr = dst + strlen(dst);

	while(*src != '\0') {
		*ptr = *src++;
	}

	*ptr = '\0';

	return dst;
}