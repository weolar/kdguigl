#include <UIlib.h>
#include <stdlib.h>
#include "../kdgui/wtf/FastMalloc.h"
#include "SkFixed.h"
#include "SkErrorInternals.h"
#include "SkUtils.h"

void SkDebugf(const char format[], ...)
{
}

void sk_free(void* p)
{
	WTF::fastFree(p);
}

void sk_throw()
{
	//SkDEBUGFAIL("sk_throw");
	abort();
}

void sk_out_of_memory(void)
{
	//SkDEBUGFAIL("sk_out_of_memory");
	abort();
}

void* sk_malloc_flags(size_t size, unsigned flags)
{
	return WTF::fastMalloc(size);
}

void* sk_malloc_throw(size_t size)
{
	return sk_malloc_flags(size, 2);
}

void* sk_realloc_throw(void* addr, size_t size)
{
	return WTF::fastRealloc(addr, size);
}

void SkErrorInternals::SetError(SkError code, const char *fmt, ...)
{
	abort();
}

SkMemset16Proc SkMemset16GetPlatformProc() {
	abort();
// 	if (cachedHasSSE2()) {
// 		return sk_memset16_SSE2;
// 	} else {
		return NULL;
//	}
}

SkMemset32Proc SkMemset32GetPlatformProc() {
	abort();
// 	if (cachedHasSSE2()) {
// 		return sk_memset32_SSE2;
// 	} else {
		return NULL;
//	}
}