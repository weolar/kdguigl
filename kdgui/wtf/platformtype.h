
#ifndef kmq_platform_h
#define kmq_platform_h

//////////////////////////////////////////////////////////////////////////

#define THREAD_NULL			0

#ifdef OS_ANDROID

#include <unistd.h>
#include <android/log.h>
#include "sys/atomics.h"

#define THREAD_RESULT_TYPE	void *
#define CREATE_THREAD(Handle, Func, Param)	{int err; pthread_t ntid; err = pthread_create(&Handle, NULL, Func, Param); if (err != THREAD_NULL) return E_FAIL;}
#define TERMINATE_THREAD(Handle)	{if (THREAD_NULL == Handle) {pthread_kill(Handle, SIGKILL); Handle = -1; }}

#define  LOG_TAG    "JNIMsg"
#define  LOGI(...)  do {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);} while(0);

#define STDMETHODCALLTYPE 
#define TRUE 1
#define FALSE 0

#define KMQ_SECOND 1

#define __in 
#define __out 
#define __out_opt 
#define __in_opt 
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

#define GENERIC_READ (0x80000000L)
#define FILE_SHARE_READ 0x00000001
#define OPEN_EXISTING 3
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define GENERIC_WRITE (0x40000000L)
#define CREATE_ALWAYS 2
#define OPEN_ALWAYS 4

typedef long HRESULT;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef long BOOL;
typedef DWORD *LPDWORD;
typedef const void* LPCVOID;

typedef const char* LPCSTR, *PCSTR;

typedef unsigned char BYTE;

#define interface struct

typedef void* LPVOID;

typedef long LONG;

typedef unsigned int UINT;
typedef unsigned char* LPBYTE;

typedef struct HWND__ * HWND;
typedef struct HDC__* HDC;

typedef long LRESULT;

typedef int WPARAM;
typedef int LPARAM;

typedef DWORD COLORREF;

typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT;

typedef struct tagPOINTS {
	short x;
	short y;
} POINTS, *PPOINTS, *LPPOINTS;

typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE;

typedef struct tagRECT {
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
} RECT;

typedef char TCHAR, *PTCHAR;

typedef unsigned short WORD;

typedef unsigned short WCHAR;

typedef const wchar_t* LPCWSTR, *PCWSTR;

#ifdef UNICODE
typedef LPCWSTR				LPCTSTR;
#else
typedef LPCSTR				LPCTSTR;
#endif

typedef unsigned long ULONG_PTR, *PULONG_PTR;

//typedef pthread_t OS_THREAD_HANDLE;

#ifndef WINAPI
#define WINAPI
#endif

#define E_FAIL (0x80000008L)
#define S_OK ((HRESULT)0L)

#define KAS_BEGIN_COM_MAP( CComImpl ) 
#define KAS_COM_INTERFACE_ENTRY( I ) 
#define KAS_END_COM_MAP() 

#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
};
typedef struct _GUID GUID;

class IUnknown {
public:
#if 0
	virtual int STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ const GUID* riid,
		/* [annotation][iid_is][out] */ 
		void **ppvObject) = 0;

	virtual ULONG STDMETHODCALLTYPE AddRef( void) = 0;

	virtual ULONG STDMETHODCALLTYPE Release( void) = 0;
#endif
};

//////////////////////////////////////////////////////////////////////////

#define INFINITE 0xFFFFFFFF 

inline LONG InterlockedIncrement(LONG volatile *Addend )
{
	return (LONG)__atomic_inc((volatile int *)Addend);
}

inline LONG InterlockedDecrement(LONG volatile *Addend )
{
	return (LONG)__atomic_dec((volatile int *)Addend);
}

inline void OutputDebugString(const char* out) {
	LOGI(out);
}

unsigned long GetTickCount();

namespace WTF {

struct Event;

} // WTF

typedef WTF::Event* OS_EVENT_HANDLE;

//////////////////////////////////////////////////////////////////////////

#define _stricmp    strcasecmp
#define _strnicmp   strncasecmp

#define _tcslen     strlen

#define _tcsicmp    strcasecmp

#define _tsscanf    sscanf

#define _ttof       atof

#define _tcstod     strtod

#define _tcscmp     strcmp

#else

#define THREAD_RESULT_TYPE	DWORD WINAPI
#define CREATE_THREAD(Handle, Func, Param)	{Handle = CreateThread(NULL, 0, Func, Param, 0, NULL); if (Handle == THREAD_NULL) return E_FAIL;}
#define TERMINATE_THREAD(Handle)	{if (Handle != THREAD_NULL) {TerminateThread(Handle, 0); CloseHandle(Handle); Handle = NULL; }}

#define _tsscanf swscanf_s
#define _tcstod wcstod

#define _ttof _wtof

#define  LOG_TAG    "JNIMsg"
#define  LOGI(...)  do {} while (0);

typedef HANDLE OS_THREAD_HANDLE;

typedef HANDLE OS_EVENT_HANDLE;

inline void KmqWaitThread(OS_EVENT_HANDLE hEvent)
{
	::WaitForSingleObject(hEvent, INFINITE);
}

inline BOOL KmqCloseThreadHandle(OS_EVENT_HANDLE hEvent)
{
	return ::CloseHandle(hEvent);
}

#define KMQ_SECOND 1000

inline void KmqSleep(int seconds)
{
	::Sleep(seconds);
}

#endif // OS_ANDROID

#endif // kmq_platform_h
