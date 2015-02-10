
#include <UIlib.h>

namespace WTF {

const int kMaxTlsKeySize = 0x1000;

void** g_tlsBuf = 0;
BOOL* g_tlsBufUsing = 0;

long& ThreadSpecTlsKeyCount() {
	static long count = 0;
	return count;
}

void ThreadSpecificInitTls() {
	if (!g_tlsBuf) {
		g_tlsBuf = new LPVOID[kMaxTlsKeySize];
		memset(g_tlsBuf, 0, sizeof(LPVOID)*kMaxTlsKeySize);
	}

	if (!g_tlsBufUsing) {
		g_tlsBufUsing = new BOOL[kMaxTlsKeySize];
		memset(g_tlsBufUsing, 0, sizeof(BOOL)*kMaxTlsKeySize);
	}
}

LPVOID WinTlsTryGetValue(__in DWORD dwTlsIndex) {
	ASSERT(dwTlsIndex < kMaxTlsKeySize)
	return g_tlsBuf[dwTlsIndex];
}

BOOL WinTlshasInit(__in DWORD dwTlsIndex) {
	ASSERT(dwTlsIndex < kMaxTlsKeySize);
	return g_tlsBufUsing[dwTlsIndex];
}

LPVOID WinTlsGetValue( __in DWORD dwTlsIndex ) {
	ASSERT(dwTlsIndex < kMaxTlsKeySize);
	ASSERT(TRUE == g_tlsBufUsing[dwTlsIndex]);

	return g_tlsBuf[dwTlsIndex];
}

BOOL WinTlsSetValue( __in DWORD dwTlsIndex, __in_opt LPVOID lpTlsValue, BOOL bIsPtr ) {
	ASSERT(dwTlsIndex < kMaxTlsKeySize);
	if (bIsPtr)
		ASSERT(FALSE == g_tlsBufUsing[dwTlsIndex]);
	g_tlsBuf[dwTlsIndex] = lpTlsValue;
	g_tlsBufUsing[dwTlsIndex] = TRUE;

	return TRUE;
}

BOOL WinTlsSetValueNone( __in DWORD dwTlsIndex, BOOL bHasInit, BOOL bIsPtr ) {
	ASSERT(dwTlsIndex < kMaxTlsKeySize);
	if (bIsPtr)
		ASSERT(bHasInit == g_tlsBufUsing[dwTlsIndex]);
	g_tlsBuf[dwTlsIndex] = NULL;
	g_tlsBufUsing[dwTlsIndex] = TRUE;
}

void WinTlsDestroyValue() {
	delete[] g_tlsBuf;
	delete[] g_tlsBufUsing;
}

DWORD WinTlsAlloc() {
	DWORD tlsIdx = InterlockedIncrement(&ThreadSpecTlsKeyCount());
	if (kMaxTlsKeySize < tlsIdx)
		return 0xffffffff;

	return tlsIdx;
}

void ThreadSpecificThreadExit() {

}

} // WTF