
#include "ThreadSpecific.h"

namespace WTF {

long& ThreadSpecTlsKeyCount()
{
	static long count = 0;
	return count;
}

static DWORD s_winTlsBufIdx = TlsAlloc();
static DWORD s_winTlsBufUsingIdx = TlsAlloc();

static LPVOID* GetOrInitOurThreadTls(BOOL*& tlsBufUsing)
{
	LPVOID* tlsBuf = (LPVOID*)TlsGetValue(s_winTlsBufIdx);
	tlsBufUsing = (BOOL*)TlsGetValue(s_winTlsBufUsingIdx);
	if (!tlsBuf) {
		tlsBuf = new LPVOID[kMaxTlsKeySize];
		memset(tlsBuf, 0, sizeof(LPVOID)*kMaxTlsKeySize);
		BOOL bSuc = TlsSetValue(s_winTlsBufIdx, tlsBuf);
		ASSERT(bSuc);

		ASSERT(!tlsBufUsing);
		tlsBufUsing = new BOOL[kMaxTlsKeySize];
		memset(tlsBufUsing, 0, sizeof(BOOL)*kMaxTlsKeySize);
		bSuc = TlsSetValue(s_winTlsBufUsingIdx, tlsBufUsing);
		ASSERT(bSuc);
	}

	return tlsBuf;
}

void ThreadSpecificInitTls()
{
	if (s_winTlsBufIdx == TLS_OUT_OF_INDEXES || s_winTlsBufUsingIdx == TLS_OUT_OF_INDEXES)
		DebugBreak();

	BOOL* tlsBufUsing = NULL;
	GetOrInitOurThreadTls(tlsBufUsing);
}

LPVOID WinTlsTryGetValue( __in DWORD dwTlsIndex )
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);
	ASSERT(dwTlsIndex < kMaxTlsKeySize);

	LPVOID* tlsBuf = (LPVOID*)TlsGetValue(s_winTlsBufIdx);
	BOOL* tlsBufUsing = (BOOL*)TlsGetValue(s_winTlsBufUsingIdx);
	if (tlsBuf && tlsBufUsing && tlsBufUsing[dwTlsIndex])
		return tlsBuf[dwTlsIndex];

	return NULL;    
}

BOOL WinTlshasInit( __in DWORD dwTlsIndex )
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);
	ASSERT(dwTlsIndex < kMaxTlsKeySize);

	BOOL* tlsBufUsing = (BOOL*)TlsGetValue(s_winTlsBufUsingIdx);
	ASSERT(tlsBufUsing);

	return tlsBufUsing[dwTlsIndex];
}

LPVOID WinTlsGetValue( __in DWORD dwTlsIndex )
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);
	ASSERT(dwTlsIndex < kMaxTlsKeySize);

	LPVOID* tlsBuf = (LPVOID*)TlsGetValue(s_winTlsBufIdx);
	BOOL* tlsBufUsing = (BOOL*)TlsGetValue(s_winTlsBufUsingIdx);
	ASSERT(tlsBuf && tlsBufUsing);
	ASSERT(TRUE == tlsBufUsing[dwTlsIndex]);

	return tlsBuf[dwTlsIndex];
}

BOOL WinTlsSetValue( __in DWORD dwTlsIndex, __in_opt LPVOID lpTlsValue, BOOL bIsPtr )
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);
	ASSERT(dwTlsIndex < kMaxTlsKeySize);

	BOOL* tlsBufUsing = NULL;
	LPVOID* tlsBuf = GetOrInitOurThreadTls(tlsBufUsing);

	ASSERT(tlsBuf);

	if (bIsPtr)
	{ ASSERT(FALSE == tlsBufUsing[dwTlsIndex]); }

	tlsBuf[dwTlsIndex] = lpTlsValue;
	tlsBufUsing[dwTlsIndex] = TRUE;

	return TRUE;
}

BOOL WinTlsSetValueNone( __in DWORD dwTlsIndex, BOOL bHasInit, BOOL bIsPtr )
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);
	ASSERT(dwTlsIndex < kMaxTlsKeySize);

	BOOL* tlsBufUsing = NULL;
	LPVOID* tlsBuf = GetOrInitOurThreadTls(tlsBufUsing);

	ASSERT(tlsBuf);

	if (bIsPtr)
		ASSERT(bHasInit == tlsBufUsing[dwTlsIndex]);

	tlsBuf[dwTlsIndex] = NULL;
	tlsBufUsing[dwTlsIndex] = TRUE;

	return TRUE;
}

void WinTlsDestroyValue()
{
	ASSERT(-1 != s_winTlsBufIdx);
	ASSERT(-1 != s_winTlsBufUsingIdx);

	LPVOID* tlsBuf = (LPVOID*)TlsGetValue(s_winTlsBufIdx);
	BOOL* tlsBufUsing = (BOOL*)TlsGetValue(s_winTlsBufUsingIdx);
	delete[] tlsBuf;
	delete[] tlsBufUsing;

	TlsSetValue(s_winTlsBufIdx, 0);
	TlsSetValue(s_winTlsBufUsingIdx, tlsBufUsing);
}

DWORD WinTlsAlloc(void)
{
	DWORD tlsIdx = InterlockedIncrement(&ThreadSpecTlsKeyCount()) - 1;
	if (kMaxTlsKeySize < tlsIdx)
		return TLS_OUT_OF_INDEXES;
	return tlsIdx;
}

void ThreadSpecificThreadExit()
{
	for (long i = 0; i < ThreadSpecTlsKeyCount(); i++) {
		// The layout of ThreadSpecific<T>::Data does not depend on T. So we are safe to do the static cast to ThreadSpecific<int> in order to access its data member.
		ThreadSpecific<int>::Data* data = static_cast<ThreadSpecific<int>::Data*>(WinTlsGetValue(i));
		if (data)
			data->destructor(data);
	}
	WinTlsDestroyValue();
}

} // WTF