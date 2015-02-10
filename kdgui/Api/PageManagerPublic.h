#ifndef PageManagerPublic_h
#define PageManagerPublic_h

#include "script/include/squirrel.h"
#include "Core/UIManager.h"

// 包装类，这样CPageManager有改动，也不需要重编译外界代码，同时可以明确哪些接口是暴露给外部调用
class CPageManager;
class PageManagerPublic {
public:

	void Init(HWND hWnd);
	void Uninit();

	bool LoadSyncScriptFromSrc(const CStdString& src);
	bool LoadAsyncScriptFromSrc(const CStdString& src);

	HSQUIRRELVM GetVM();

// 	void TimerFired();
// 	void ResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 
// 	void PaintEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 	void MouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 	void CaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 	void KillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 
// 	int  InputEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 	bool InputEventToRichEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	BOOL ProcessDbgMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	HWND GetHWND();

	void SetBackgroundColor(COLORREF c);
	bool HasJsonReady();

	void* GetForeignPtr();

	void CopyMemoryDC(HDC hDC, const RECT* rc);

	void Invalidate(const IntRect& rc);

	KWebApiCallbackSet& GetCallback();

	static PageManagerPublic* CreateWindowByRealWnd(const WCHAR* className, KdPageInfoPtr pageInfo, void* foreignPtr);


#ifdef _MSC_VER
protected:
	friend CPageManager;
#endif
	PageManagerPublic(CPageManager* ptr);
protected:
	CPageManager* m_ptr;
};

#endif // PageManagerPublic_h