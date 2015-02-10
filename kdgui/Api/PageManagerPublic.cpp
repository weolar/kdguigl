#include "PageManagerPublic.h"
#include "Core/UIManager.h"

PageManagerPublic::PageManagerPublic(CPageManager* ptr) {
	m_ptr = ptr;
}

void PageManagerPublic::Init(HWND hWnd) {
	m_ptr->Init(hWnd);
}

void PageManagerPublic::Uninit() {
	m_ptr->Uninit();
}

bool PageManagerPublic::LoadSyncScriptFromSrc(const CStdString& src) {
	return m_ptr->LoadSyncScriptFromSrc(src);
}

bool PageManagerPublic::LoadAsyncScriptFromSrc(const CStdString& src) {
	return m_ptr->LoadAsyncScriptFromSrc(src);
}

HSQUIRRELVM PageManagerPublic::GetVM() {
	return m_ptr->GetVM();
}

BOOL PageManagerPublic::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
	return m_ptr->PreProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
}

BOOL PageManagerPublic::ProcessDbgMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
	return m_ptr->ProcessDbgMessage(hWnd, uMsg, wParam, lParam, lResult);
}

HWND PageManagerPublic::GetHWND() {
	return m_ptr->GetHWND();
}

void PageManagerPublic::SetBackgroundColor(COLORREF c) {
	m_ptr->SetBackgroundColor(c);
}

bool PageManagerPublic::HasJsonReady() {
	return m_ptr->HasJsonReady();
}

void* PageManagerPublic::GetForeignPtr() {
	return m_ptr->GetPagePtr();
}

KWebApiCallbackSet& PageManagerPublic::GetCallback() {
	return m_ptr->m_callbacks;
}

void PageManagerPublic::CopyMemoryDC(HDC hDC, const RECT* rc) {
	m_ptr->CopyMemoryDC(hDC, rc);
}

void PageManagerPublic::Invalidate(const IntRect& rc) {
	m_ptr->Invalidate(rc);
}

PageManagerPublic* PageManagerPublic::CreateWindowByRealWnd(const WCHAR* className, KdPageInfoPtr pageInfo, void* foreignPtr) {
	CPageManager* page = 0;
	page = new CPageManager();
	page->SetPagePtr(foreignPtr);
	page->SaveScriptDebugInfo(&pageInfo->DebugInfo);

	//pageInfo->dwExStyle &= ~WS_EX_LAYERED;
	page->SetLayerWindow(!!(pageInfo->dwExStyle & (WS_EX_LAYERED)));

	HWND hWnd = 0;
	hWnd = CreateWindowExW(pageInfo->dwExStyle, className, L"", pageInfo->dwStyle,
		pageInfo->X, pageInfo->Y, pageInfo->nWidth, pageInfo->nHeight, 
		pageInfo->hWndParent, NULL, NULL, (LPVOID)page->GetWrap());
	KDASSERT(!!hWnd);
	KDASSERT(!!page);

	PageManagerPublic* self = reinterpret_cast<PageManagerPublic*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!self) { // 有可能没收到WM_NCCREATE消息
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(page->GetWrap()));
		page->GetWrap()->Init(hWnd);
	}
	
	return page->GetWrap();
}