#include "string.h"


const char* strstr(const char* X, const char* Y) {
	while(*X != '\0') {
		if((*X == *Y) && compare(X,Y)) {
			return X;
		}
		X++;
		return NULL;
	}
}