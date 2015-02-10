/*
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007, 2008, 2009 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <WTF/RefCountedLeakCounter.h>
#include <WTF/UtilHelp.h>
#include <WTF/OffScreenDC.h>
#include <WTF/InjectTool.h>

#include "RichEditBaseNode.h"
#include "RenderRichEditWin.h"
#include "RenderRichEditTimer.h"

#include "core/UIManager.h"
#include "graphics/GraphicsContext.h"
#include "graphics/PlatformContextSkia.h"

//#define USE_MY_DLL 1
#define USE_MY_LIB 1

#ifdef USE_MY_DLL
#define  RICHEDIT_DLL_NAME L"F:\\app_prj\\Gui2\\re\\Debug\\re.dll"
#else
#define  RICHEDIT_DLL_NAME L"Riched20.dll"
#endif

#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "lib/InjectTool.lib")

#if USE_MY_LIB

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

#else

#ifdef _DEBUG
#pragma comment(lib,"lib/KdRichEdit_d.lib")
#else
#pragma comment(lib,"lib/KdRichEdit.lib")
#endif

extern "C" HRESULT __stdcall KdCreateTextServices(
    IUnknown *punkOuter,
    ITextHost *pITextHost, 
    IUnknown **ppUnk
    );

extern "C" BOOL WINAPI KdDllMain(HMODULE hmod, DWORD dwReason, LPVOID lpvReserved);

LONG DXtoHimetricX(LONG dx, LONG xPerInch);

#endif

// // Convert Himetric along the X axis to X pixels
// LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch) {
// 	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
// }
// 
// // Convert Himetric along the Y axis to Y pixels
// LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch) {
// 	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
// }

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HFONT WINAPI MyCreateFontIndirectW( __in CONST LOGFONTW *lplf) {
	return ::CreateFontIndirectW(lplf);
}

namespace WebCore {
namespace FontChromiumWinNs {

BOOL __stdcall AlphaAwareExtTextOut(
	__in  HDC hdc,
	__in  int x,
	__in  int y,
	__in  UINT options,
	__in  const RECT *lpRect,
	__in  LPCWSTR lpString,
	__in  UINT count,
	__in  const INT *lpDx
	) {
	RECT rect = *lpRect;
	FillRect(hdc, lpRect, (HBRUSH) (COLOR_WINDOW+1));

// 	HFONT hFont = (HFONT)GetStockObject(SYSTEM_FONT);
// 
// 	LOGFONT lf;
// 	::GetObject(hFont, sizeof(LOGFONT), &lf);
// 
// 	HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
// 
// 	LOGFONT oldLF;
// 	::GetObject(hOldFont, sizeof(LOGFONT), &oldLF);
// 
// 	DrawText(hdc, lpString, count, &rect, DT_LEFT);
// 	return TRUE;

	return ExtTextOut(hdc, x, y, options, lpRect, lpString, count, lpDx);
}

}
}

// namespace WebCore {

static HWND GetHWNDByNode(RichEditBaseNode* richEditNode) {
    return richEditNode->GetManager()->GetHWND();
}

#ifndef NDEBUG
static RefCountedLeakCounter KRenderSVGRichEditImplCounter("RenderRichEdit");
#endif

RenderRichEdit::RenderRichEdit(RichEditBaseNode* pEdit) {
#ifndef NDEBUG
    KRenderSVGRichEditImplCounter.increment();
#endif
    m_nRef = 0;
    m_richEditNode = pEdit;

    m_lastMouseUpTime = 0;

    m_pTextService = NULL;
    m_dwStyle = 0;

    m_bCanRepaint = TRUE;
    m_bClicntRectDirty = TRUE;

    m_fEnableAutoWordSel= 0;
    m_fWordWrap	= 0;
    m_fAllowBeep = TRUE;
    m_fRich = 0;
    m_fSaveSelection = 0;
    m_fInplaceActive = 0;
    m_fTransparent = 0;
    m_fTimer = 0;
    m_fCaptured = 0;

    m_lSelBarWidth = 0;
    m_cchTextMost = cInitTextMax;
    m_dwEventMask = 0;
    m_icf = 0;
    m_ipf = 0;
    ZeroMemory(&m_rcClient, sizeof(m_rcClient));
    m_sizelExtent.cx = m_sizelExtent.cy = 0;
    ZeroMemory(&m_cf, sizeof(m_cf));
    ZeroMemory(&m_pf, sizeof(m_pf));
    m_laccelpos = -1;
    m_chPasswordChar = L'*';

    m_bCaretShow = FALSE;
    m_bHaveMove = FALSE;
    m_offScreen = NULL;
    m_bIsTransparencyLayer = FALSE;

    ZeroMemory(&m_rtInset, sizeof(m_rtInset));
    ZeroMemory(&m_rtInsetHimetric, sizeof(m_rtInsetHimetric));

	m_bCreateSysCaret = false;

	ThreadTimers* threadTimers = m_richEditNode->GetManager()->GetThreadTimers();
	m_caretTimer = new Timer<RenderRichEdit>(this, &RenderRichEdit::CaretFire, threadTimers);
}

RenderRichEdit::~RenderRichEdit() {
    UnInit();
#ifndef NDEBUG
    KRenderSVGRichEditImplCounter.decrement();
#endif
}

BOOL RenderRichEdit::InitDefaultCharFormat() {
    CHARFORMAT2W* pcf = &m_cf;
    memset(pcf, 0, sizeof(CHARFORMAT2W));

    LOGFONT lf = {0};
	if (0 == ::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf))
		return FALSE;

	lf.lfItalic = FALSE;
	lf.lfHeight = -16;
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName, L"微软雅黑");

    COLORREF clrColor = RGB(0,0,0); // pTextStyle->color(); //RGB(0,0,255);

	HDC hDC = ::GetDC(NULL);
    LONG yPixPerInch = ::GetDeviceCaps(hDC, LOGPIXELSY);
    ::ReleaseDC(NULL, hDC);

    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = clrColor;

	pcf->wWeight = lf.lfWeight;

    pcf->yHeight = -lf.lfHeight / 1.3125; // -lf.lfHeight * LY_PER_INCH / yPixPerInch;

    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_FACE | CFM_CHARSET | CFM_COLOR;
    if(lf.lfWeight >= FW_BOLD)
        pcf->dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        pcf->dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
    wcscpy_s(pcf->szFaceName, lf.lfFaceName);

    return TRUE;
}

BOOL RenderRichEdit::InitDefaultParaFormat() {
    memset(&m_pf, 0, sizeof(PARAFORMAT2));
    m_pf.cbSize = sizeof(PARAFORMAT2);
    m_pf.dwMask = PFM_ALL;
    m_pf.wAlignment = PFA_LEFT;
    m_pf.cTabCount = 1;
    m_pf.rgxTabs[0] = lDefaultTab;

    return TRUE;
}

void RenderRichEdit::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
	HWND hWnd = GetHWNDByNode(m_richEditNode);
	IntRect rtControl;
	GetLocalControlRect(rtControl);

	HDC hdc = skia::BeginPlatformPaint(g->platformContext()->canvas());

	XFORM xf = {0};
	XFORM xfEmpty = {1, 0, 0, 1, 0, 0};
	BOOL bSuc;
	bSuc = GetWorldTransform(hdc, &xf);
	bSuc = SetWorldTransform(hdc, &xfEmpty);

	AffineTransform trans = g->getCTM();

	IntRect updateRect = rtControl;
	updateRect.intersect(rcInvalid);
	updateRect = trans.mapRect(updateRect);
	IntRect rtControlIntRect = trans.mapRect(rtControl);

	if (m_bIsTransparencyLayer) {
		DrawLayer(g, hdc, rtControl, rtControlIntRect, updateRect);
	} else {
		m_pTextService->TxDraw(
			DVASPECT_CONTENT,  		           // Draw Aspect
			0,						           // Lindex
			NULL,                              // Info for drawing optimazation (被我用来标识分层窗口了，其实是OCX 96)
			NULL,					           // target device information
			hdc,                               // Draw device HDC
			NULL, 				   	           // Target device HDC
			(RECTL*)&(RECT)(rtControlIntRect), // Bounding client rectangle
			NULL, 		                       // Clipping rectangle for metafiles
			(RECT*)&((RECT)updateRect),        // Update rectangle
			NULL, 	   				           // Call back function
			NULL,					           // Call back parameter
			0);				                   // What view of the object
	}

	if (m_bCaretShow)
		g->drawRect(m_caretRect);

	bSuc = SetWorldTransform(hdc, &xf);
	skia::EndPlatformPaint(g->platformContext()->canvas());
}

#if !USE_MY_LIB
//static WTF::ThreadSpecificVal<bool> s_bCreateFun;
static bool s_bCreateFun = false;
#endif

BOOL RenderRichEdit::Init(const CREATESTRUCT *pcs) {
    m_bHaveMove = FALSE;
	m_bIsTransparencyLayer = m_richEditNode->GetManager()->IsLayerWindow();

    if(!InitDefaultCharFormat()) return FALSE;
    if(!InitDefaultParaFormat()) return FALSE;

    if (pcs != NULL) {
        m_dwStyle |= pcs->style;

        if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
            m_fWordWrap = TRUE;
    }

    IUnknown* pUnk = NULL;
    
    HRESULT hr;
    // Create Text Services component
#if USE_MY_LIB
    HINSTANCE hModRichEdit = ::LoadLibraryW(RICHEDIT_DLL_NAME);
    if(!hModRichEdit) {
        ASSERT(FALSE);
        return FALSE;
    }

	ReplaceIATFunc("Riched20.dll", "Gdi32.dll", "ExtTextOutW", &WebCore::FontChromiumWinNs::AlphaAwareExtTextOut, NULL);
	ReplaceIATFunc("Riched20.dll", "Gdi32.dll", "CreateFontIndirectW", &MyCreateFontIndirectW, NULL);
	
	//Rookits(hModRichEdit, "Gdi32.dll", "ExtTextOut", &WebCore::FontChromiumWinNs::AlphaAwareExtTextOut);

#	ifdef USE_MY_DLL
    PCreateTextServices pCreateFun = (PCreateTextServices)::GetProcAddress(hModRichEdit, "_CreateTextServices@12");
#	else
    PCreateTextServices pCreateFun = (PCreateTextServices)::GetProcAddress(hModRichEdit, "CreateTextServices");
#	endif

#else
    PCreateTextServices pCreateFun = KdCreateTextServices;
    if (!s_bCreateFun) {
        s_bCreateFun = true;
        KdDllMain(NULL, DLL_PROCESS_ATTACH, NULL);
    }
#endif
    if(FAILED(pCreateFun(NULL, this, &pUnk))) return FALSE;
    ASSERT(m_pTextService == NULL);
    ASSERT(pUnk != NULL);
    if(pUnk != NULL) {
        hr = pUnk->QueryInterface(IID_ITextServices,(void **)&m_pTextService);
        pUnk->Release();
    }

    if(FAILED(hr)) {
        ASSERT(FALSE);
        return FALSE;
    }
    ASSERT(m_pTextService != NULL);

    // Set window text
    if(pcs != NULL && pcs->lpszName != NULL && m_pTextService != NULL) {
        m_pTextService->TxSetText((TCHAR *)pcs->lpszName);
        OnTxInPlaceActivate(NULL);
        m_pTextService->TxSendMessage(EM_SETEVENTMASK, 0, m_dwEventMask, 0);
    }

	//SetFont((HFONT)GetStockObject(SYSTEM_FONT));

    return TRUE;
}

void RenderRichEdit::UnInit() {
    if(m_pTextService != NULL) {
        OnTxInPlaceDeactivate();
        m_pTextService->Release();
        m_pTextService = NULL;
    }

	UHDeletePtr(&m_caretTimer);
	UHDeletePtr(&m_offScreen);

	for (WTF::HashMap<UINT, RenderRichEditTimeData*>::iterator it = m_timers.begin(); it != m_timers.end(); ++it) {
		UHDeletePtr(&it->second->timer);
        fastFree(it->second);
	}
    m_timers.clear();

    m_richEditNode = NULL;
}

//////////////////////////////////////////////////////////////////////////

void LoadTransformToDC(HDC dc, const AffineTransform& matrix) {
    XFORM xf;
    //SkMatrix xxx = matrix;

    xf.eM11 = (float)matrix.a();
    xf.eM12 = (float)matrix.b();
    xf.eM21 = (float)matrix.c();
    xf.eM22 = (float)matrix.d();
    xf.eDx = (float)matrix.e();
    xf.eDy = (float)matrix.f();
    SetWorldTransform(dc, &xf);
}

void RenderRichEdit::SetOrKillFocus(BOOL b) {
    LRESULT result = 0;
	UINT msg = b ? WM_SETFOCUS : WM_KILLFOCUS;
	m_pTextService->TxSendMessage(msg, 0, 0, &result);
}

void RenderRichEdit::DrawLayer(GraphicsContext* g, HDC hdc, 
	const IntRect& rtControl, const IntRect& rtControlIntRect, const IntRect& updateRect) {
	if (!m_offScreen) {
		m_offScreen = new OffScreenDC();
		m_offScreen->Init(hdc, rtControl.width(), rtControl.height());
	}

	if (m_bClicntRectDirty) {
		IntRect localRect(0, 0, rtControlIntRect.width(), rtControlIntRect.height());
		IntRect localUpdateRect = updateRect;
		localUpdateRect.move(-rtControlIntRect.x(), -rtControlIntRect.y());

		m_offScreen->ZeroMem();

		m_pTextService->TxDraw(
			DVASPECT_CONTENT,  		           // Draw Aspect
			0,						           // Lindex
			(void *)1,                         // Info for drawing optimazation (被我用来标识分层窗口了，其实是OCX 96)
			NULL,					           // target device information
			m_offScreen->m_hdc,                // Draw device HDC
			NULL, 				   	           // Target device HDC
			(RECTL*)&(RECT)(localRect),//localRect        // Bounding client rectangle
			NULL, 		                       // Clipping rectangle for metafiles
			(RECT*)&((RECT)localUpdateRect),//localUpdateRect   // Update rectangle
			NULL, 	   				           // Call back function
			NULL,					           // Call back parameter
			0);				                   // What view of the object
	}
	m_bClicntRectDirty = FALSE;

	BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hdc, rtControlIntRect.x(), rtControlIntRect.y(), rtControlIntRect.width(), rtControlIntRect.height(),
		m_offScreen->m_hdc, 0, 0, rtControlIntRect.width(), rtControlIntRect.height(), blend);
}

LRESULT RenderRichEdit::Event(TEventUI* event) {
    LRESULT lResult = 0;

	UINT message = 0;
	POINT& pt = event->ptMouse;

    if (UHIsMouseEvent(event->Type)) {
        if (UIEVENT_MOUSEDOWN == event->Type && 0 == event->button) {
            message = WM_LBUTTONDOWN;
            m_bHaveMove = FALSE;
        } else if (UIEVENT_MOUSEUP == event->Type && 0 == event->button) {
			if (event->dwTimestamp - m_lastMouseUpTime < 200)
				message = WM_LBUTTONDBLCLK;
			else
				message = WM_LBUTTONUP;
			
			m_bHaveMove = FALSE;
			m_lastMouseUpTime = event->dwTimestamp;
        } else if (UIEVENT_MOUSEDOWN == event->Type && 2 == event->button) {
            message = WM_RBUTTONDOWN;
            m_bHaveMove = FALSE;
        } else if (UIEVENT_MOUSEUP == event->Type && 2 == event->button) {
            message = WM_RBUTTONUP;
            m_bHaveMove = FALSE;
        } else if (UIEVENT_MOUSEMOVE == event->Type) {
            if (!m_bHaveMove /*|| !mouseEvent->buttonDown()*/)
                DoSetCursor(NULL, &pt);
            m_bHaveMove = TRUE;
            message = WM_MOUSEMOVE;
        }

        if (message)
            m_pTextService->TxSendMessage(message, 0, MAKELONG(pt.x, pt.y), &lResult);

        return lResult;
    } else if (0 != event->uMsg) {
        if (WM_SETCURSOR == event->uMsg)
            return lResult;
        
        // 处理输入法消息时，S_FALSE 表示要调用def函数，否则就不调用
        // Message was not processed by text services so send it
        // to the default window proc.
        event->bNotCallDefWindowProc = S_FALSE == m_pTextService->TxSendMessage(event->uMsg, event->wParam, event->lParam, &lResult);
    }

    return lResult;
}

/////////////////// IUnknow  /////////////////////////////
HRESULT RenderRichEdit::QueryInterface(REFIID riid, void **ppvObject) {
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)) {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG RenderRichEdit::AddRef(void) {
    return 1;
}

ULONG RenderRichEdit::Release(void) {
    return 1;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC RenderRichEdit::TxImmGetContext(void) {
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    if(hWnd != NULL)
        return ::ImmGetContext(hWnd);

	return NULL;
}

void RenderRichEdit::TxImmReleaseContext(HIMC himc) {
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    if(hWnd != NULL)
		::ImmReleaseContext(hWnd, himc);
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC RenderRichEdit::TxGetDC() {
    ASSERT(m_richEditNode != NULL);
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    ASSERT(hWnd != NULL && ::IsWindow(hWnd));

    if(hWnd != NULL && ::IsWindow(hWnd))
        return ::GetDC(hWnd);

	return NULL;
}

int RenderRichEdit::TxReleaseDC(HDC hdc) {
    ASSERT(m_richEditNode != NULL);
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    ASSERT(hWnd != NULL && ::IsWindow(hWnd));

    if(hWnd != NULL && ::IsWindow(hWnd))
		return ::ReleaseDC(hWnd, hdc);

	return 0;
}

static void SendSrollMessage(RichEditBaseNode* pRichEdit) {
    //if (pRichEdit && pRichEdit->frame()->view() && pRichEdit->frame()->document()) {
    //    pRichEdit->frame()->document()->eventQueue()->enqueueOrDispatchScrollEvent(pRichEdit->node(), EventQueue::ScrollEventDocumentTarget);}
}

BOOL RenderRichEdit::TxShowScrollBar(INT fnBar, BOOL fShow) {
    IntRect rc = GetClientRect();
    if (rc.isEmpty())
        return TRUE;
    
    if(fnBar == SB_HORZ) {
        if (TRUE == fShow && m_HScrollData.bShow != fShow) {
            if (rc.height() < m_scrollHeight)
                KDASSERT(FALSE);
            rc.setHeight(rc.height() - m_scrollHeight);
        } else if (FALSE == fShow && m_HScrollData.bShow != fShow) {
            rc.setHeight(rc.height() + m_scrollHeight);
        }
        m_HScrollData.bShow = fShow;
    } else {
        if (TRUE == fShow && m_HScrollData.bShow != fShow) {
            if (rc.width() < m_scrollWidth)
                KDASSERT(FALSE);
            rc.setWidth(rc.width() - m_scrollWidth);
        } else if (FALSE == fShow && m_HScrollData.bShow != fShow) {
            rc.setWidth(rc.width() + m_scrollWidth);
        }
        m_VScrollData.bShow = fShow;
    }

    SetClientRect(rc);
    SendSrollMessage(m_richEditNode);

    return TRUE;
}

BOOL RenderRichEdit::TxEnableScrollBar (INT fnBar, INT fuArrowflags) {
    SendSrollMessage(m_richEditNode);
    return TRUE;
}

BOOL RenderRichEdit::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {
    if(fnBar == SB_HORZ) {
        m_HScrollData.nMinPos = nMinPos;
        m_HScrollData.nMaxPos = nMaxPos;
    } else {
        m_VScrollData.nMinPos = nMinPos;
        m_VScrollData.nMaxPos = nMaxPos;
    }
    SendSrollMessage(m_richEditNode);

    return TRUE;
}

BOOL RenderRichEdit::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw) {
    if(fnBar == SB_HORZ) {
        m_HScrollData.nPos = nPos;
    } else {
        m_VScrollData.nPos = nPos;
    }

    SendSrollMessage(m_richEditNode);
    return TRUE;
}

void RenderRichEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode) {
    InvalidateRect(prc, fMode, TRUE);
}

void RenderRichEdit::InvalidateRect(LPCRECT prc, BOOL fMode, BOOL bClicntRectDirty) {
    if (!m_bCanRepaint)
        return;

    if (bClicntRectDirty) // false == bClicntRectDirty 都是游标重绘导致的，为了不覆盖真正的重绘请求，故这里判断下
        m_bClicntRectDirty = TRUE;

    IntRect rtUpdate;
    if(prc == NULL)
        GetControlRect(rtUpdate); // m_rcClient;
    else
        rtUpdate = *prc;
    
    if (rtUpdate.isEmpty())
        return;

    ASSERT(m_richEditNode != NULL);
	m_richEditNode->Invalidate();
}

void RenderRichEdit::TxViewChange(BOOL fUpdate) {
    TxInvalidateRect(NULL, TRUE);
}

void RenderRichEdit::CaretFire(Timer<RenderRichEdit>*) {
    m_bCaretShow = !m_bCaretShow;
    InvalidateRect(&(RECT)m_caretRect, TRUE, FALSE);
}

BOOL RenderRichEdit::CreateSysCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {
	if (m_bCreateSysCaret)
		return TRUE;
	m_bCreateSysCaret = true;

	::DestroyCaret();
	::SetCaretBlinkTime(500);

	HWND hWnd = GetHWNDByNode(m_richEditNode);
	ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(!hWnd)
		return FALSE;

	if (!::CreateCaret(hWnd, hbmp, xWidth, yHeight))
		return FALSE;

	::ShowCaret(hWnd);

	return TRUE;
}

BOOL RenderRichEdit::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {
    m_caretRect.setWidth(xWidth);
    m_caretRect.setHeight(yHeight);

    InvalidateRect(&(RECT)m_caretRect, TRUE, FALSE);
#if 0
	if (!m_bIsTransparencyLayer)
		return CreateSysCaret(hbmp, xWidth, yHeight);
#endif
	m_caretTimer->stop();
	m_caretTimer->start(0.5, 0.5);

	return TRUE;
}

// 这个地方容易出点小问题，主要是刷新的
BOOL RenderRichEdit::TxSetCaretPos(INT x, INT y) {
	IntRect localRect;
	GetLocalControlRect(localRect);
	if (x < localRect.x())
		x = localRect.x();

	if (y < localRect.y())
		y = localRect.y();
	
    InvalidateRect(&(RECT)m_caretRect, TRUE, !m_bIsTransparencyLayer);

    m_caretRect.setX(x);
    m_caretRect.setY(y);
#if 0
    if (m_bIsTransparencyLayer)
		return ::SetCaretPos(x, y);
#endif
	InvalidateRect(&(RECT)m_caretRect, TRUE, FALSE);
	return TRUE;
}

BOOL RenderRichEdit::TxShowCaret(BOOL fShow)
{
	ASSERT(m_richEditNode != NULL);
	HWND hWnd = GetHWNDByNode(m_richEditNode);
	ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(!hWnd) 
		return FALSE;

	// 调用多少次HideCaret，就必须调用多少次ShowCaret才能显示
	m_bCaretShow = fShow;
#if 0
	if(fShow) {
		::ShowCaret(hWnd);
	} else {
		::HideCaret(hWnd);
	}
#endif

	m_caretTimer->stop();
	if (fShow)
		m_caretTimer->start(0.5, 0.5);

	if (!fShow && !m_caretRect.isEmpty())
		InvalidateRect(&(RECT)m_caretRect, TRUE, FALSE);

	return TRUE;
}

void RenderRichEdit::RichEditTimeMsgDispatch(RenderRichEditTimer* timer) {
	WTF::HashMap<UINT, RenderRichEditTimeData*>::iterator it = m_timers.find(timer->idTimer);
	if (m_timers.end() == it)
		return;

	RenderRichEditTimeData* pData = it->second;
	ITextServices* pTextServ = GetTextServices();
	LRESULT result = 0;

	if (pTextServ)
		pTextServ->TxSendMessage(WM_TIMER, pData->idTimer, 0, &result);
}

BOOL RenderRichEdit::TxSetTimer(UINT idTimer, UINT uTimeout) {
	WTF::HashMap<UINT, RenderRichEditTimeData*>::iterator it = m_timers.find(idTimer);
	if (m_timers.end() != it) {
		it->second->timer->start(uTimeout, uTimeout);
		return TRUE;
	}

    RenderRichEditTimeData* pData = (RenderRichEditTimeData*)fastMalloc(sizeof(RenderRichEditTimeData));
    pData->idTimer = idTimer;

	ThreadTimers* threadTimers = m_richEditNode->GetManager()->GetThreadTimers();
	pData->timer = new RenderRichEditTimer(this, &RenderRichEdit::RichEditTimeMsgDispatch, threadTimers);
    pData->timer->start(uTimeout, uTimeout);

    m_timers.insert(idTimer, pData);

    m_fTimer = TRUE;
    return m_fTimer;
}

void RenderRichEdit::TxKillTimer(UINT idTimer) {
	WTF::HashMap<UINT, RenderRichEditTimeData*>::iterator it = m_timers.find(idTimer);
	if (m_timers.end() != it) {
		it->second->timer->stop();
		UHDeletePtr(&it->second->timer);
		fastFree(it->second);
		m_timers.remove(it);
	}

    m_fTimer = FALSE;
}

void RenderRichEdit::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	
    LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) {
    return;
}

void RenderRichEdit::TxSetCapture(BOOL fCapture) {
    ASSERT(m_richEditNode != NULL);
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    ASSERT(hWnd != NULL && ::IsWindow(hWnd));

    if(hWnd != NULL && ::IsWindow(hWnd)) {
        if (fCapture)
			::SetCapture(hWnd);
        else
			::ReleaseCapture();

        m_fCaptured = fCapture;
    }
}

void RenderRichEdit::TxSetFocus() {
    ASSERT(m_richEditNode != NULL);

	m_richEditNode->SetFocus();
}

void RenderRichEdit::TxSetCursor(HCURSOR hcur, BOOL fText) {
    ::SetCursor(hcur);
}

BOOL RenderRichEdit::TxScreenToClient(LPPOINT lppt) {
    ASSERT(m_richEditNode != NULL);
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    ASSERT(hWnd != NULL && ::IsWindow(hWnd));

    if(hWnd != NULL && ::IsWindow(hWnd))
        return ::ScreenToClient(hWnd, lppt);
    
    return FALSE;
}

BOOL RenderRichEdit::TxClientToScreen(LPPOINT lppt) {
    ASSERT(m_richEditNode != NULL);
    HWND hWnd = GetHWNDByNode(m_richEditNode);
    ASSERT(hWnd != NULL && ::IsWindow(hWnd));

    if(hWnd != NULL && ::IsWindow(hWnd))
        return ::ClientToScreen(hWnd, lppt);

    return FALSE;
}

HRESULT RenderRichEdit::TxActivate(LONG *plOldState) {
    return S_OK;
}

HRESULT RenderRichEdit::TxDeactivate(LONG lNewState) {
    return S_OK;
}

HRESULT RenderRichEdit::TxGetClientRect(LPRECT prc) {
    IntRect rc;
    GetControlRect(rc);
    *prc = rc;

    return NOERROR;
}

HRESULT RenderRichEdit::TxGetViewInset(LPRECT prc) {
    *prc = m_rtInsetHimetric;
    return NOERROR;	
}

HRESULT RenderRichEdit::TxGetCharFormat(const CHARFORMATW **ppCF) {
	return E_NOTIMPL;

    *ppCF = &m_cf;
    return NOERROR;
}

HRESULT RenderRichEdit::TxGetParaFormat(const PARAFORMAT **ppPF) {
    *ppPF = &m_pf;
    return NOERROR;
}

COLORREF RenderRichEdit::TxGetSysColor(int nIndex) {
    if (COLOR_HIGHLIGHT == nIndex) // add by weolar
        //return RGB(193,232,195); // 豆沙绿
        return RGB(50,100,195);
        //return RGB(150,150,249);

    if (COLOR_HIGHLIGHTTEXT == nIndex) // 高亮文本色
        return RGB(253,205,196);
    
    return ::GetSysColor(nIndex);
}

HRESULT RenderRichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle) {
    *pstyle = !m_fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT RenderRichEdit::TxGetMaxLength(DWORD *pLength) {
    *pLength = m_cchTextMost;
    return NOERROR;
}

HRESULT RenderRichEdit::TxGetScrollBars(DWORD *pdwScrollBar) {
    *pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}

HRESULT RenderRichEdit::TxGetPasswordChar(TCHAR *pch) {
    *pch = m_chPasswordChar;
    return NOERROR;
}

HRESULT RenderRichEdit::TxGetAcceleratorPos(LONG *pcp) {
    *pcp = m_laccelpos;
    return S_OK;
} 										   

HRESULT RenderRichEdit::OnTxCharFormatChange(const CHARFORMATW *pcf) {
    return S_OK;
}

HRESULT RenderRichEdit::OnTxParaFormatChange(const PARAFORMAT *ppf) {
    return S_OK;
}

HRESULT RenderRichEdit::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) {
    DWORD dwProperties = 0;

    if (m_fRich)
        dwProperties = TXTBIT_RICHTEXT;

    if (m_dwStyle & ES_MULTILINE)
        dwProperties |= TXTBIT_MULTILINE;

    if (m_dwStyle & ES_READONLY)
        dwProperties |= TXTBIT_READONLY;
    
    if (m_dwStyle & ES_PASSWORD)
        dwProperties |= TXTBIT_USEPASSWORD;

    if (!(m_dwStyle & ES_NOHIDESEL))
		dwProperties |= TXTBIT_HIDESELECTION;

    if (m_fEnableAutoWordSel)
        dwProperties |= TXTBIT_AUTOWORDSEL;

    if (m_fWordWrap)
        dwProperties |= TXTBIT_WORDWRAP;

    if (m_fAllowBeep)
        dwProperties |= TXTBIT_ALLOWBEEP;

    if (m_fSaveSelection)
        dwProperties |= TXTBIT_SAVESELECTION;

    *pdwBits = (dwProperties & dwMask); 

    return NOERROR;
}

HRESULT RenderRichEdit::TxNotify(DWORD iNotify, void *pv) {
    ASSERT(m_richEditNode != NULL);

    return S_OK;
}

HRESULT RenderRichEdit::TxGetExtent(LPSIZEL lpExtent) {
    *lpExtent = m_sizelExtent;
    return S_OK;
}

HRESULT	RenderRichEdit::TxGetSelectionBarWidth (LONG *plSelBarWidth) {
    *plSelBarWidth = m_lSelBarWidth;
    return S_OK;
}

////////////////////// helper functions ///////////////////////////////
VOID RenderRichEdit::GetViewInset(RECT& rt) const {
    rt = m_rtInset;
}

VOID RenderRichEdit::SetViewInset(const RECT& rt) {
    m_rtInset = rt;

    HDC hDC = GetDC(NULL);
    LONG xPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(hDC, LOGPIXELSY); 
    ReleaseDC(NULL, hDC);

    m_rtInsetHimetric.left = DXtoHimetricX(rt.left, xPerInch);
    m_rtInsetHimetric.right = DXtoHimetricX(rt.right, xPerInch);
    m_rtInsetHimetric.top = DYtoHimetricY(rt.top, yPerInch);
    m_rtInsetHimetric.bottom = DYtoHimetricY(rt.bottom, yPerInch);

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

void RenderRichEdit::SetWordWrap(BOOL fWordWrap) {
    m_fWordWrap = fWordWrap;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL RenderRichEdit::GetReadOnly() {
    return (m_dwStyle & ES_READONLY) != 0;
}

void RenderRichEdit::SetReadOnly(BOOL fReadOnly) {
    if (fReadOnly)
        m_dwStyle |= ES_READONLY;
    else
        m_dwStyle &= ~ES_READONLY;

    if(m_pTextService != NULL) {
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_READONLY, 
            fReadOnly ? TXTBIT_READONLY : 0);
    }
}

void RenderRichEdit::SetFont(HFONT hFont) {
    if( hFont == NULL ) return;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);

    HDC dc = GetDC(NULL);
    LONG yPixPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
	::ReleaseDC(NULL, dc);

    m_cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if(lf.lfWeight >= FW_BOLD)
        m_cf.dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        m_cf.dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        m_cf.dwEffects |= CFE_UNDERLINE;
    m_cf.bCharSet = lf.lfCharSet;
    m_cf.bPitchAndFamily = lf.lfPitchAndFamily;
    wcscpy_s(m_cf.szFaceName, lf.lfFaceName);

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
}

void RenderRichEdit::SetColor(COLORREF clrText) {
    m_cf.crTextColor = clrText;
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
}

SIZEL* RenderRichEdit::GetExtent() {
    return &m_sizelExtent;
}

void RenderRichEdit::SetExtent(SIZEL *psizelExtent) { 
    m_sizelExtent = *psizelExtent; 
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void RenderRichEdit::LimitText(LONG nChars) {
    m_cchTextMost = nChars;
    if( m_cchTextMost <= 0 ) m_cchTextMost = cInitTextMax;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

LONG RenderRichEdit::GetLimitText() {
    return m_cchTextMost;
}

BOOL RenderRichEdit::IsCaptured() {
    return m_fCaptured;
}

BOOL RenderRichEdit::GetAllowBeep() {
    return m_fAllowBeep;
}

void RenderRichEdit::SetAllowBeep(BOOL fAllowBeep) {
    m_fAllowBeep = fAllowBeep;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
            fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD RenderRichEdit::GetDefaultAlign() {
    return m_pf.wAlignment;
}

void RenderRichEdit::SetDefaultAlign(WORD wNewAlign) {
    m_pf.wAlignment = wNewAlign;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL RenderRichEdit::GetRichTextFlag() {
    return m_fRich;
}

void RenderRichEdit::SetRichTextFlag(BOOL fNew) {
    m_fRich = fNew;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
}

VOID RenderRichEdit::SetEventMask(DWORD dwMask)
{
    m_dwEventMask = dwMask;
    if(m_pTextService != NULL)
        m_pTextService->TxSendMessage(EM_SETEVENTMASK, 0, m_dwEventMask, 0);
}

DWORD RenderRichEdit::GetEventMask() const {
    return m_dwEventMask;
}

LONG RenderRichEdit::GetDefaultLeftIndent() {
    return m_pf.dxOffset;
}

void RenderRichEdit::SetDefaultLeftIndent(LONG lNewIndent) {
    m_pf.dxOffset = lNewIndent;
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void RenderRichEdit::GetControlRect(IntRect& rc) {
    rc = m_richEditNode->BoundingRectAbsolute();
}

void RenderRichEdit::GetLocalControlRect(IntRect& rc) {
	if (!m_rcClient.isEmpty())
		rc = m_rcClient;
	GetControlRect(rc);
}

void RenderRichEdit::SetClientRect(const IntRect& rc) {
    if (m_rcClient == rc && rc.isEmpty())
        return;
    
    m_rcClient = rc;
    if (m_offScreen)
        m_offScreen->Realloc(m_rcClient.width(), m_rcClient.height());
    
    HDC hDC = GetDC(NULL);
    LONG xPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(hDC, LOGPIXELSY); 
    ReleaseDC(NULL, hDC);

    m_sizelExtent.cx = DXtoHimetricX(m_rcClient.width(), xPerInch);
    m_sizelExtent.cy = DYtoHimetricY(m_rcClient.height(), yPerInch);

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
}

BOOL RenderRichEdit::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;
    m_fSaveSelection = f_SaveSelection;

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, m_fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;		
}

HRESULT	RenderRichEdit::OnTxInPlaceDeactivate() {
    HRESULT hr(E_FAIL);
    if(m_pTextService != NULL)
        hr	= m_pTextService->OnTxInPlaceDeactivate();

    if (SUCCEEDED(hr))
        m_fInplaceActive = FALSE;

    return hr;
}

HRESULT	RenderRichEdit::OnTxInPlaceActivate(LPCRECT prcClient) {
    m_fInplaceActive = TRUE;
    HRESULT hr(E_FAIL);
    if(m_pTextService != NULL)
        hr = m_pTextService->OnTxInPlaceActivate(prcClient);

    if (FAILED(hr))
        m_fInplaceActive = FALSE;

    return hr;
}

BOOL RenderRichEdit::DoSetCursor(RECT *prc, POINT *pt) {
    m_pTextService->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL, NULL, NULL, NULL, pt->x, pt->y);

    return FALSE;
}

void RenderRichEdit::SetTransparent(BOOL f_Transparent) {
    m_fTransparent = f_Transparent;
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG RenderRichEdit::SetAccelPos(LONG l_accelpos) {
    LONG laccelposOld = l_accelpos;
    m_laccelpos = l_accelpos;
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

    return laccelposOld;
}

WCHAR RenderRichEdit::SetPasswordChar(WCHAR ch_PasswordChar) {
    WCHAR chOldPasswordChar = m_chPasswordChar;
    m_chPasswordChar = ch_PasswordChar;
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
            (m_chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

    return chOldPasswordChar;
}

void RenderRichEdit::SetDisabled(BOOL fOn) {
    m_cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
    m_cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if( !fOn )
        m_cf.dwEffects &= ~CFE_DISABLED;
    
    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
}

LONG RenderRichEdit::SetSelBarWidth(LONG l_SelBarWidth) {
    LONG lOldSelBarWidth = m_lSelBarWidth;

    m_lSelBarWidth = l_SelBarWidth;

    if (m_lSelBarWidth)
        m_dwStyle |= ES_SELECTIONBAR;
    else
        m_dwStyle &= (~ES_SELECTIONBAR);

    if(m_pTextService != NULL)
        m_pTextService->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

BOOL RenderRichEdit::GetTimerState() {
    return m_fTimer;
}

void RenderRichEdit::SetCharFormat(CHARFORMAT2W &c) {
    m_cf = c;
    if(m_pTextService != NULL)
        m_pTextService->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&m_cf, 0);
}

void RenderRichEdit::SetParaFormat(PARAFORMAT2 &p) {
    m_pf = p;
}

// } // namespace WebCore