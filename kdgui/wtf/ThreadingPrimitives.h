#ifndef ThreadingPrimitives_h
#define ThreadingPrimitives_h

#include "Locker.h"

namespace WTF {

#ifdef _MSC_VER

struct PlatformMutex {
	CRITICAL_SECTION m_internalMutex;
	size_t m_recursionCount;
};
	
class Mutex {
WTF_MAKE_NONCOPYABLE(Mutex); WTF_MAKE_FAST_ALLOCATED;
public:
	Mutex();
	~Mutex();

	void lock();
	bool tryLock();
	void unlock();

public:
	PlatformMutex& impl() { return m_mutex; }
private:
	PlatformMutex m_mutex;
};

#else

#include <pthread.h>
#include <asm-generic/errno-base.h>

class Mutex
{
public:
	inline Mutex ()
	{
		int rc = pthread_mutex_init (&mutex, NULL);
		//posix_assert (rc);
	}

	inline ~Mutex ()
	{
		int rc = pthread_mutex_destroy (&mutex);
		//posix_assert (rc);
	}

	inline void lock ()
	{
		int rc = pthread_mutex_lock (&mutex);
		//posix_assert (rc);
	}

	inline bool tryLock ()
	{
		int rc = pthread_mutex_trylock (&mutex);
		if (rc == EBUSY)
			return false;

		//posix_assert (rc);
		return true;
	}

	inline void unlock ()
	{
		int rc = pthread_mutex_unlock (&mutex);
		//posix_assert (rc);
	}

private:

	pthread_mutex_t mutex;

	// Disable copy construction and assignment.
	Mutex (const Mutex&);
	const Mutex &operator = (const Mutex&);
};

#endif // _MSC_VER

typedef Locker<Mutex> MutexLocker;

} // WTF

#endif // ThreadingPrimitives_h