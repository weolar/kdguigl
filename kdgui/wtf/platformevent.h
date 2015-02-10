
namespace WTF {

BOOL SetEvent(OS_EVENT_HANDLE hEvent);

OS_EVENT_HANDLE CreateEvent(BOOL bManualReset);

BOOL CloseEventHandle(OS_EVENT_HANDLE hEvent);

BOOL WaitEvent(OS_EVENT_HANDLE hEvent, DWORD dwMilliseconds);

} // WTF
