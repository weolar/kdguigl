/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"

namespace WTF {
    void* fastMalloc(size_t size);
    void* fastRealloc(void*, size_t);
    void fastFree(void* p);
}

int sq_mem = 0;
void *sq_vm_malloc(SQUnsignedInteger size)
{
    //return malloc(size);
    sq_mem += size;
    return WTF::fastMalloc(size);
}

void *sq_vm_realloc(void *p, SQUnsignedInteger oldsize, SQUnsignedInteger size)
{
    //return realloc(p, size);
    sq_mem -= oldsize;
    sq_mem += size;
    return WTF::fastRealloc(p, size);
}

void sq_vm_free(void *p, SQUnsignedInteger size)
{
    //free(p);
    sq_mem -= size;
    WTF::fastFree(p);
}
