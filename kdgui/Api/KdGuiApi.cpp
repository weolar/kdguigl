/*
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006 Jon Shier (jshier@iastate.edu)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reseved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *  Copyright (C) 2009 Google Inc. All rights reseved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

using std::min;
using std::max;

#include "gdiplus.h"

#include "PageManagerPublic.h"
#include "KdGuiApi.h"
#include "KdPageInfo.h"
#include <wtf/ThreadSpecific.h>
#include <wtf/HashSet.h>
#include <wtf/MainThread.h>
#include "script/include/squirrel.h"

static ATOM _RegisterClass(HINSTANCE hInstance);
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static ULONG_PTR g_nGdiplusToken = 0;

const WCHAR* szWindowClass = L"Internet Explorer Server";

typedef PageManagerPublic* KdPagePtr;
WTF::ThreadSpecific<WTF::HashSet<KdPagePtr>> g_KdPageSet;

class TestA {
public:
	~TestA();

	void test() {
		aa = 1;
	}
	int aa;
};

class TestB {
public:
	~TestB() {
		;
	}
	int BB;
};

WTF::ThreadSpecific<TestB> g_TestBB;

TestA::~TestA() {
	if (!g_TestBB.hasInit())
		g_TestBB = new TestB();
}

KDEXPORT KdGuiObjPtr KDCALL
KdCreateGuiObj(void* pForeignPtr) {
    MTInit();
    _RegisterClass(0);
    return NULL;
}

KDEXPORT KdPagePtr KDCALL
KdCreateRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPageInfoPtr pPageInfo,
    void* pForeignPtr,
    bool bShow
    )
{
    _RegisterClass(0);

    KdPagePtr page = PageManagerPublic::CreateWindowByRealWnd(szWindowClass, pPageInfo, pForeignPtr);
    if (bShow) {
        ::ShowWindow(page->GetHWND(), SW_NORMAL);
        ::UpdateWindow(page->GetHWND());
    }

    if (g_KdPageSet.get()->contains(page))
        DebugBreak();
    g_KdPageSet.get()->add(page);

    return page;
}

KDEXPORT HWND KDCALL
KdGetHWNDFromPagePtr(KdPagePtr kdPage)
{
    return kdPage->GetHWND();
}

KDEXPORT void KDCALL
KdCloseRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPagePtr pPage
    )
{
    HWND hWnd = pPage->GetHWND();
    pPage->Uninit();

    ::CloseWindow(hWnd); // 这里面会post一个close消息,负责 delete pPage;
}

KDEXPORT bool KDCALL
KdLoadPageFormUrl(KdPagePtr kdPageHandle, LPCWSTR lpUrl)
{
    kdPageHandle->LoadAsyncScriptFromSrc(lpUrl);
    return true;
}

KDEXPORT bool KDCALL
KdLoadPageFormData(KdPagePtr kdPageHandle, const void* lpData, int nLen)
{
	DebugBreak();
    //kdPageHandle->loadFormData(lpData, nLen);
    return true;
}

KDEXPORT void KDCALL
KdPostResToAsynchronousLoad(KdPagePtr kdPageHandle, const WCHAR* pUrl, void* pResBuf, int nResBufLen, bool bNeedSavaRes)
{
	DebugBreak();
    //kdPageHandle->postResToAsynchronousLoad(pUrl, pResBuf, nResBufLen, bNeedSavaRes);
}

KDEXPORT void KDCALL
KdGetMainVM(KdGuiObjPtr kdHandle)
{
	DebugBreak();
}

KDEXPORT HSQUIRRELVM KDCALL
KdGetPageVM(KdPagePtr kdPageHandle)
{
    // 如果页面没调用到脚本，脚本可能没被初始化。不过windowShell会帮助做这事情
    return kdPageHandle->GetVM();
}

KDEXPORT void KDCALL
KdRegisterMsgHandle(KdPagePtr kdPageHandle, PFN_KdPageWinMsgCallback pPreCallBack, PFN_KdPageWinMsgCallback pPostCallBack)
{
    kdPageHandle->GetCallback().m_msgPreCallBack = pPreCallBack;
    kdPageHandle->GetCallback().m_msgPostCallBack = pPostCallBack;
}

// 注册资源处理回调, 比如读取本地一个文件的时候回调
KDEXPORT void KDCALL
KdRegisterResHandle(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack)
{
     kdPageHandle->GetCallback().m_resHandle = pCallBack;
}

// 注册资源别名查询，一般用在调试脚本时
KDEXPORT void KDCALL
KdRegisterResOtherNameQuery(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack)
{
    kdPageHandle->GetCallback().m_resOtherNameQuery = pCallBack;
}

// 注册绘图回调
KDEXPORT void KDCALL
KdRegisterPaintCallback(KdPagePtr kdPageHandle, PFN_KdPagePaintCallback pCallBack)
{
    kdPageHandle->GetCallback().m_paint = pCallBack;
}

KDEXPORT void KDCALL
KdRepaintRequested(KdPagePtr kdPageHandle, const RECT* repaintRect)
{
    kdPageHandle->Invalidate(IntRect(*repaintRect));
}

// 注册XML解析完毕时候webcore发回的回调，相当于入口函数
KDEXPORT void KDCALL
KdRegisterXMLOnRealy(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack)
{
    kdPageHandle->GetCallback().m_xmlHaveFinished = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterUninit(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack)
{
    kdPageHandle->GetCallback().m_unintCallBack = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterScriptInit(KdPagePtr kdPageHandle, PFN_KdPageScriptInitCallback pCallBack)
{
    kdPageHandle->GetCallback().m_scriptInitCallBack = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterError2(KdPagePtr kdPageHandle, SQPRINTFUNCTION pCallBack)
{
    kdPageHandle->GetCallback().m_error = (PFN_KdPageError)pCallBack;
}

//////////////////////////////////////////////////////////////////////////
 
KDEXPORT void KDCALL KdCopyMemoryDC(KdPagePtr kdPageHandle, HDC hDC, const RECT* rc)
{
    kdPageHandle->CopyMemoryDC(hDC, rc);
}

KDEXPORT void KDCALL
KdDeletePageNotCloseWindow(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
    if (!g_KdPageSet.get() || !g_KdPageSet.get()->contains(kdPageHandle)) {
        if (IsDebuggerPresent())
            DebugBreak();
        return;
    }
    g_KdPageSet.get()->remove(kdPageHandle);

    kdPageHandle->Uninit();
    delete kdPageHandle;
}

KDEXPORT int KDCALL
KdSendInputEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
    //return kdPageHandle->InputEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL KdSetBackgroundColor(KdPagePtr kdPageHandle, COLORREF c)
{
    kdPageHandle->SetBackgroundColor(c);
}

KDEXPORT void KDCALL KdShowDebugNodeData(KdPagePtr kdPageHandle)
{
	DebugBreak();
    //kdPageHandle->DumpNode();
}

KDEXPORT BOOL KDCALL KdHasJsonReady(KdPagePtr kdPageHandle)
{
    return kdPageHandle->HasJsonReady() ? TRUE : FALSE;
}

KDEXPORT void KDCALL
KdInitThread()
{
	static TestA aa;
	aa.test();

    MTInit();

	if (!g_nGdiplusToken) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&g_nGdiplusToken, &gdiplusStartupInput,NULL);
	}

    if (!g_KdPageSet.hasInit())
        g_KdPageSet.set(new WTF::HashSet<KdPagePtr>);
}

KDEXPORT void KDCALL
KdUninitThread()
{
//    WTF::ThreadSpecificThreadExit();
}

KDEXPORT void KDCALL
KdFireSharedTimer()
{
    //WebCore::SharedTimerKd::inst()->timerEvent();
}

KDEXPORT int KDCALL KdSettingChange(KdPagePtr kdPageHandle, const WCHAR* settingType, void* setting)
{
	if (0 == settingType)
		return 0;
#if 0
	if (0 == wcsicmp(settingType, L"ErrorHandling"))
		g_sq_error_handling = (int)setting;
#endif
	return 0;
}

static ATOM _RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= _WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName	= szWindowClass;

    return RegisterClassEx(&wcex);
}

#if 0
#define OUTPUTSTR(x) OutputDebugStringW(x)
#else
#define OUTPUTSTR(x) 
#endif

static bool IsInputEvent(UINT message)
{
    switch (message) {
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
            //OutputDebugStringW(L"IsInputEvent\n");
            return true;
            
        case WM_IME_STARTCOMPOSITION:OUTPUTSTR(L"WM_IME_STARTCOMPOSITION\n");return true;
        case WM_IME_ENDCOMPOSITION:OUTPUTSTR(L"WM_IME_ENDCOMPOSITION\n");return true;
        case WM_IME_COMPOSITION:OUTPUTSTR(L"WM_IME_COMPOSITION\n");return true;
        case WM_IME_SETCONTEXT:OUTPUTSTR(L"WM_IME_SETCONTEXT\n");return true;
        case WM_IME_NOTIFY:OUTPUTSTR(L"WM_IME_NOTIFY\n");return true;
        case WM_IME_CONTROL:OUTPUTSTR(L"WM_IME_CONTROL\n");return true;
        case WM_IME_COMPOSITIONFULL:OUTPUTSTR(L"WM_IME_COMPOSITIONFULL\n");return true;
        case WM_IME_SELECT:OUTPUTSTR(L"WM_IME_SELECT\n");return true;
        case WM_IME_CHAR:OUTPUTSTR(L"WM_IME_CHAR\n");return true;
        case WM_IME_REQUEST:OUTPUTSTR(L"WM_IME_REQUEST\n");return true;
        case WM_IME_KEYDOWN:OUTPUTSTR(L"WM_IME_KEYDOWN\n");return true;
        case WM_IME_KEYUP:OUTPUTSTR(L"WM_IME_KEYUP\n");return true;
    }
    
    return false;
}

static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool bNeedCallDefWindowProc = true;
    KdPagePtr pKdGUIPage = 0;
    LRESULT lResult = 0;

    if(WM_NCCREATE == message) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pKdGUIPage = static_cast<KdPagePtr>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pKdGUIPage));
        
        pKdGUIPage->Init(hWnd);
    } else
        pKdGUIPage = reinterpret_cast<KdPagePtr>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (!pKdGUIPage)
        return DefWindowProc(hWnd, message, wParam, lParam);

    PFN_KdPageWinMsgCallback postMsgCallBack = pKdGUIPage->GetCallback().m_msgPostCallBack;
    void* pMainContext = NULL;
    void* pPageContext = pKdGUIPage->GetForeignPtr();

	if (pKdGUIPage->ProcessDbgMessage(hWnd, message, wParam, lParam, lResult))
		return 0;

    if (pKdGUIPage->GetCallback().m_msgPreCallBack) {
        lResult = pKdGUIPage->GetCallback().m_msgPreCallBack(pKdGUIPage, NULL, 
            pKdGUIPage->GetForeignPtr(),
            hWnd, message, wParam, lParam, &bNeedCallDefWindowProc);

        if (false == bNeedCallDefWindowProc)
            return lResult;
    }

	bNeedCallDefWindowProc = !pKdGUIPage->ProcessWindowMessage(hWnd, message, wParam, lParam, lResult);

	if (WM_NCDESTROY == message) {
		KdDeletePageNotCloseWindow(pKdGUIPage, hWnd, message, wParam, lParam);
		pKdGUIPage = 0;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pKdGUIPage));
	}

    if (postMsgCallBack && pKdGUIPage)
        lResult = postMsgCallBack(pKdGUIPage, pMainContext, pPageContext, hWnd, message, wParam, lParam, &bNeedCallDefWindowProc);

    if (false == bNeedCallDefWindowProc)
        return lResult;

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LPCWSTR KdGetVersion()
{
    return L"KdGui2#2014.2.11";
}

//////////////////////////////////////////////////////////////////////////

KVAPtr KVACreate(int iElementSize, int iPreallocSize)
{
    return new CStdValArray(iElementSize, iPreallocSize);
}

void KVADelete(KVAPtr pThis)
{
    delete pThis;
}

void KVAEmpty(KVAPtr pThis)
{
    ((CStdValArray*)pThis)->Empty();
}

bool KVAIsEmpty(KVAPtr pThis)
{
    return ((CStdValArray*)pThis)->IsEmpty();
}

bool KVAAdd(KVAPtr pThis, LPCVOID pData)
{
    return ((CStdValArray*)pThis)->Add(pData);
}

bool KVARemove(KVAPtr pThis, int iIndex)
{
    return ((CStdValArray*)pThis)->Remove(iIndex);
}

bool KVAResize(KVAPtr pThis, int iIndex)
{
    return ((CStdValArray*)pThis)->ResizePrealloc(iIndex);
}

int KVAGetSize(KVAPtr pThis)
{
    return ((CStdValArray*)pThis)->GetSize();
}

void KVASetSize(KVAPtr pThis, int nSize)
{
    ((CStdValArray*)pThis)->SetSize(nSize);
}

LPVOID KVAGetData(KVAPtr pThis)
{
    return ((CStdValArray*)pThis)->GetAt(0);
}

LPVOID KVAGetAt(KVAPtr pThis, int iIndex)
{
    return ((CStdValArray*)pThis)->GetAt(iIndex);
}

BOOL APIENTRY DllMain( 
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}