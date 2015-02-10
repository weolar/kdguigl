
#ifdef _MSC_VER
#include <windows.h>
#else
#include <uilib.h>
#include <pthread.h>
#include <errno.h>
#include "platformevent.h"
#endif

namespace WTF {

#ifdef _MSC_VER

BOOL SetEvent(OS_EVENT_HANDLE hEvent) {
	return ::SetEvent(hEvent);
}

OS_EVENT_HANDLE CreateEvent(BOOL bManualReset) {
	return ::CreateEvent(NULL, bManualReset, FALSE, NULL);
}

BOOL CloseEventHandle(OS_EVENT_HANDLE hEvent) {
	return ::CloseHandle(hEvent);
}

BOOL WaitEvent(OS_EVENT_HANDLE hEvent, DWORD dwMilliseconds) {
	return ::WaitForSingleObject(hEvent, dwMilliseconds);
}

#else

struct Event {
	BOOL            m_manual;  
	volatile BOOL   m_state;  
	pthread_mutex_t m_mutex;  
	pthread_cond_t  m_cond;  
};

BOOL SetEvent(OS_EVENT_HANDLE hEvent) {
	pthread_mutex_lock(&hEvent->m_mutex);

	//设置状态变量为true，对应有信号
	hEvent->m_state = true;

	//重新激活所有在等待m_cond变量的线程
	if (pthread_cond_broadcast(&hEvent->m_cond))
		pthread_mutex_unlock(&hEvent->m_mutex);
	
	pthread_mutex_unlock(&hEvent->m_mutex);

	return TRUE;
}

OS_EVENT_HANDLE CreateEvent(BOOL bManualReset) {
	Event* event = new Event();
	event->m_manual = bManualReset;
	event->m_state = false;

	pthread_mutex_init(&event->m_mutex, NULL); 
	pthread_cond_init(&event->m_cond, NULL);
	
	return event;
}

BOOL CloseEventHandle(OS_EVENT_HANDLE hEvent) {
	pthread_cond_destroy(&hEvent->m_cond);
	pthread_mutex_destroy(&hEvent->m_mutex);

	return TRUE;
}

BOOL WaitEventInfinite(OS_EVENT_HANDLE hEvent) {
	if (pthread_mutex_lock(&hEvent->m_mutex))
		return FALSE;

	while (!hEvent->m_state) {
		//cout<<"CEventImpl::WaitImpl while m_state = "<<m_state<<endl;   

		//对互斥体进行原子的解锁工作,然后等待状态信号   
		if (pthread_cond_wait(&hEvent->m_cond, &hEvent->m_mutex)) {
			pthread_mutex_unlock(&hEvent->m_mutex);
			return FALSE;  
		}
	}

	if (hEvent->m_manual)
		hEvent->m_state = FALSE;
	pthread_mutex_unlock(&hEvent->m_mutex);

	return TRUE;
}

BOOL WaitEvent(OS_EVENT_HANDLE hEvent, DWORD dwMilliseconds) {
	if (INFINITE == dwMilliseconds)
		return WaitEventInfinite(hEvent);

	int rc = 0;
	struct timespec abstime;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	abstime.tv_sec  = tv.tv_sec + dwMilliseconds / 1000;
	abstime.tv_nsec = tv.tv_usec*1000 + (dwMilliseconds % 1000)*1000000;
	if (abstime.tv_nsec >= 1000000000) {
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}

	if (pthread_mutex_lock(&hEvent->m_mutex) != 0)
		return FALSE;

	while (!hEvent->m_state) {
		//自动释放互斥体并且等待m_cond状态,并且限制了最大的等待时间
		if ((rc = pthread_cond_timedwait(&hEvent->m_cond, &hEvent->m_mutex, &abstime))) {
			if (rc == ETIMEDOUT)
				break;
			pthread_mutex_unlock(&hEvent->m_mutex);
			return FALSE;  
		}
	}

	if (rc == 0 && hEvent->m_manual)
		hEvent->m_state = FALSE;
	pthread_mutex_unlock(&hEvent->m_mutex);  

	return rc == 0 ? TRUE : FALSE;
}

#endif // _MSC_VER

} // WTF