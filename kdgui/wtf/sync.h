#ifndef sync_h
#define sync_h

struct PlatformMutex {
	CRITICAL_SECTION m_internalMutex;
	size_t m_recursionCount;
};

class KdMutex {
public:
	KdMutex();
	~KdMutex();

	void Lock();
	bool TryLock();
	void Unlock();

public:
	PlatformMutex& impl() { return m_mutex; }
private:
	PlatformMutex m_mutex;
};

#endif // sync_h