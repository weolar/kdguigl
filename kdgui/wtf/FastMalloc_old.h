#ifndef FastMalloc_h
#define FastMalloc_h

inline void* fastMalloc(size_t size) {
	return malloc(size);
}

inline void fastFree(void* p) {
	return free(p);
}

inline void* fastZeroedMalloc(size_t size) {
	void* p = fastMalloc(size);
	memset(p, 0, size);
	return p;
}

#endif // FastMalloc_h