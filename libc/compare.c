#include "stdlib.h"

int compare(const char* X, const char* Y) {
	while(*X && *Y)
	{
		if(*X != *Y) {
			return 0;
		}

		X++;
		Y++;
	}

	return (*Y == '\0');
}