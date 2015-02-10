
#include "sync.h"

KdMutex::KdMutex()
{
	m_mutex.m_recursionCount = 0;
	InitializeCriticalSection(&m_mutex.m_internalMutex);
}

KdMutex::~KdMutex()
{
	DeleteCriticalSection(&m_mutex.m_internalMutex);
}

void KdMutex::Lock()
{
	EnterCriticalSection(&m_mutex.m_internalMutex);
	++m_mutex.m_recursionCount;
}

bool KdMutex::TryLock()
{
	// This method is modeled after the behavior of pthread_mutex_trylock,
	// which will return an error if the lock is already owned by the
	// current thread.  Since the primitive Win32 'TryEnterCriticalSection'
	// treats this as a successful case, it changes the behavior of several
	// tests in WebKit that check to see if the current thread already
	// owned this mutex (see e.g., IconDatabase::getOrCreateIconRecord)
	DWORD result = TryEnterCriticalSection(&m_mutex.m_internalMutex);

	if (result != 0) {       // We got the lock
		// If this thread already had the lock, we must unlock and
		// return false so that we mimic the behavior of POSIX's
		// pthread_mutex_trylock:
		if (m_mutex.m_recursionCount > 0) {
			LeaveCriticalSection(&m_mutex.m_internalMutex);
			return false;
		}

		++m_mutex.m_recursionCount;
		return true;
	}

	return false;
}

void KdMutex::Unlock()
{
	--m_mutex.m_recursionCount;
	LeaveCriticalSection(&m_mutex.m_internalMutex);
}