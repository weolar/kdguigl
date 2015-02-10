
#include <uilib.h>
#include <stdlib.h>
#include "FastMalloc.h"

namespace WTF {

void* fastMalloc(size_t n) {
	return malloc(n);
}

void fastFree(void* p) {
	free(p);
}

void* fastRealloc(void* p, size_t s) {
	return realloc(p, s);
}

} // WTF