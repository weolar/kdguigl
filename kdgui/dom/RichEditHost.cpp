/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/


/*************************************************************************
 HOST.C	-- Text Host for Windowless Rich Edit Control 
 
***************************************************************************/

#include <windows.h>
#include <stddef.h>
#include <richedit.h>
#include <memory.h>
#include <imm.h>
#include <tchar.h>

#include <WTF/UtilHelp.h>


#include "RichEditHost.h"
#include "RichEditBaseNode.h"
#include "core/UIManager.h"
#include "graphics/GraphicsContext.h"
#include "graphics/PlatformContextSkia.h"
#include "Core/UIEventType.h"

#define USE_SYS_DLL 1

#pragma comment(lib, "Imm32.lib")

#ifndef USE_SYS_DLL

#pragma comment(lib,"lib/KdRichEdit.lib")

#include <WTF/InjectTool.h>
#pragma comment(lib, "lib/InjectTool.lib")

#include "KdRichEdit/textserv.h"

#endif


HFONT WINAPI HookCreateFontIndirectW( __in CONST LOGFONTW *lplf) {
	return ::CreateFontIndirectW(lplf);
}

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
	return ExtTextOutW(hdc, x, y, options, lpRect, lpString, count, lpDx);
}

static HWND GetHWNDByNode(RichEditBaseNode* richEditNode) {
	return richEditNode->GetManager()->GetHWND();
}

BOOL fInAssert = FALSE;

// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540

#if USE_SYS_DLL

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

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

#else

LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch);
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch);
LONG DXtoHimetricX(LONG dx, LONG xPerInch);

#endif

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

EXTERN_C const IID IID_ITextEditControl = { /* f6642620-d266-11ce-a89e-00aa006cadc5 */
	0xf6642620,
	0xd266,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

const LONG cInitTextMax = (32 * 1024) - 1;
const LONG cResetTextMax = (64 * 1024);


#define ibPed 0
#define SYS_ALTERNATE 0x20000000


INT	dct;			    // Double Click Time in milliseconds
INT     nScrollInset;
COLORREF crAutoColor = 0;

LONG RichEditHost::xWidthSys = 0;    		            // average char width of system font
LONG RichEditHost::yHeightSys = 0;				// height of system font
LONG RichEditHost::yPerInch = 0;				// y pixels per inch
LONG RichEditHost::xPerInch = 0;				// x pixels per inch

HRESULT InitDefaultCharFormat(CHARFORMATW * pcf, HFONT hfont) 
{
	HWND hwnd;
	LOGFONT lf;
	HDC hdc;
	LONG yPixPerInch;

	// Get LOGFONT for default font
	if (!hfont)
		hfont = (HFONT)GetStockObject(SYSTEM_FONT);

	// Get LOGFONT for passed hfont
	if (!GetObject(hfont, sizeof(LOGFONT), &lf))
		return E_FAIL;

	lf.lfHeight = 12;
	lf.lfWeight = 400;

	// Set CHARFORMAT structure
	pcf->cbSize = sizeof(CHARFORMAT2);
	
	hwnd = GetDesktopWindow();
	hdc = GetDC(hwnd);
	yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
	pcf->yHeight = lf.lfHeight * LY_PER_INCH / yPixPerInch;
	ReleaseDC(hwnd, hdc);

	pcf->yOffset = 0;
	pcf->crTextColor = crAutoColor;

	pcf->dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
	pcf->dwEffects &= ~(CFE_PROTECTED | CFE_LINK);

	if(lf.lfWeight < FW_BOLD)
		pcf->dwEffects &= ~CFE_BOLD;
	if(!lf.lfItalic)
		pcf->dwEffects &= ~CFE_ITALIC;
	if(!lf.lfUnderline)
		pcf->dwEffects &= ~CFE_UNDERLINE;
	if(!lf.lfStrikeOut)
		pcf->dwEffects &= ~CFE_STRIKEOUT;

	pcf->dwMask = CFM_ALL | CFM_BACKCOLOR;
	pcf->bCharSet = lf.lfCharSet;
	pcf->bPitchAndFamily = lf.lfPitchAndFamily;

	//wcscpy_s(pcf->szFaceName, lf.lfFaceName);
	wcscpy_s(pcf->szFaceName, L"微软雅黑");
	
	return S_OK;
}

HRESULT InitDefaultParaFormat(PARAFORMAT * ppf) 
{	
	memset(ppf, 0, sizeof(PARAFORMAT));

	ppf->cbSize = sizeof(PARAFORMAT);
	ppf->dwMask = PFM_ALL;
	ppf->wAlignment = PFA_LEFT;
	ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

	return S_OK;
}

LRESULT MapHresultToLresult(HRESULT hr, UINT msg)
{
	LRESULT lres = hr;

	switch(msg)
	{
	case EM_GETMODIFY:
		lres = (hr == S_OK) ? -1 : 0;
		break;

		// These messages must return TRUE/FALSE rather than an hresult.
	case EM_UNDO:
	case WM_UNDO:
	case EM_CANUNDO:
	case EM_CANPASTE:
	case EM_LINESCROLL:

		// Hresults are backwards from TRUE and FALSE so we need
		// to do that remapping here as well.

		lres = (hr == S_OK) ? TRUE : FALSE;

		break;

	case EM_EXLINEFROMCHAR:
	case EM_LINEFROMCHAR:

		// If success, then hr a number. If error, it s/b 0.
		lres = SUCCEEDED(hr) ? (LRESULT) hr : 0;
		break;
			
	case EM_LINEINDEX:

		// If success, then hr a number. If error, it s/b -1.
		lres = SUCCEEDED(hr) ? (LRESULT) hr : -1;
		break;	

	default:
		lres = (LRESULT) hr;		
	}

	return lres;
}


BOOL GetIconic(HWND hwnd) 
{
	while(hwnd)
	{
		if(::IsIconic(hwnd))
			return TRUE;
		hwnd = GetParent(hwnd);
	}
	return FALSE;
}


RichEditHost::RichEditHost(RichEditBaseNode* pEdit)
{
	ZeroMemory(&pnc, sizeof(RichEditHost) - offsetof(RichEditHost, pnc));
	m_cchTextMost = cInitTextMax;
	laccelpos = -1;
	m_node = pEdit;
	m_bIsTransparencyLayer = false;
	m_bHaveMove = FALSE;
	m_lastMouseUpTime = 0;
	m_bCanRepaint = true;
	m_bClicntRectDirty = TRUE;

	m_sizelExtent.cx = 1;
	m_sizelExtent.cy = 1;
}

RichEditHost::~RichEditHost()
{
	// Revoke our drop target
	RevokeDragDrop();

	m_pserv->OnTxInPlaceDeactivate();

	m_pserv->Release();
}


////////////////////// Create/Init/Destruct Commands ///////////////////////

typedef HRESULT (STDAPICALLTYPE * PCreateTextServices)(
	IUnknown *punkOuter,
	ITextHost *pITextHost, 
	IUnknown **ppUnk);

extern "C" BOOL WINAPI KdDllMain(HMODULE hmod, DWORD dwReason, LPVOID lpvReserved);

ITextServices* GetTxtServiec(ITextHost *pITextHost) {
	HRESULT hr;
	IUnknown *pUnk = NULL;
	ITextServices* m_pserv = NULL;
#if USE_SYS_DLL
	HINSTANCE hModRichEdit = ::LoadLibraryW(L"Riched20.dll");
	PCreateTextServices pCreateFun = (PCreateTextServices)::GetProcAddress(hModRichEdit, "CreateTextServices");
#else
	PCreateTextServices pCreateFun = KdCreateTextServices;
	KdDllMain(NULL, DLL_PROCESS_ATTACH, NULL);
#endif

	if(FAILED(pCreateFun(NULL, pITextHost, &pUnk)))
		return NULL;

	if (!pUnk)
		return FALSE;

	hr = pUnk->QueryInterface(IID_ITextServices,(void **)&m_pserv);

	// Whether the previous call succeeded or failed we are done
	// with the private interface.
	pUnk->Release();

	if(FAILED(hr))
		return FALSE;
#if 0
	ReplaceIATFunc("Riched20.dll", "Gdi32.dll", "ExtTextOutW", &AlphaAwareExtTextOut, NULL);
	ReplaceIATFunc("Riched20.dll", "Gdi32.dll", "CreateFontIndirectW", &HookCreateFontIndirectW, NULL);
#endif
	return m_pserv;
}
/*
 *	RichEditHost::Init
 *
 *	Purpose:
 *		Initializes this RichEditHost
 *
 *	TODO:	Format Cache logic needs to be cleaned up. 
 */
BOOL RichEditHost::Init(
	HWND h_wnd, 
	const CREATESTRUCT *pcs,
	PNOTIFY_CALL p_nc)
{
    HDC hdc;
    HFONT hfontOld;
    TEXTMETRIC tm;
	
	// Initialize Reference count
	m_cRefs = 1;

	// Set up the notification callback
	pnc = p_nc;	
	
	m_hWnd = h_wnd;

	// Create and cache CHARFORMAT for this control
	if(FAILED(InitDefaultCharFormat(&cf, NULL)))
		return FALSE;
		
	// Create and cache PARAFORMAT for this control
	if(FAILED(InitDefaultParaFormat(&pf)))
		return FALSE;

 	// edit controls created without a window are multiline by default
	// so that paragraph formats can be
	m_dwStyle = ES_MULTILINE;
	m_fHidden = TRUE;

	// edit controls are rich by default
	m_fRich = TRUE;
	
	if(pcs)
	{
		m_hWndParent = pcs->hwndParent;
		dwExStyle = pcs->dwExStyle;
		m_dwStyle = pcs->style;

		m_fBorder = !!(m_dwStyle & WS_BORDER);

		if(m_dwStyle & ES_SUNKEN)
			m_fBorder = TRUE;

		if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
			m_fWordWrap = TRUE;
	}

	if(!(m_dwStyle & ES_LEFT))
	{
		if(m_dwStyle & ES_CENTER)
			pf.wAlignment = PFA_CENTER;
		else if(m_dwStyle & ES_RIGHT)
			pf.wAlignment = PFA_RIGHT;
	}

    // Init system metrics
	hdc = GetDC(m_hWnd);
    if(!hdc)
        return FALSE;

   	hfontOld = (HFONT)SelectObject(hdc, GetStockObject(SYSTEM_FONT));

	if(!hfontOld)
		return FALSE;

	GetTextMetrics(hdc, &tm);
	SelectObject(hdc, hfontOld);

	xWidthSys = (INT) tm.tmAveCharWidth;
    yHeightSys = (INT) tm.tmHeight;
	xPerInch = GetDeviceCaps(hdc, LOGPIXELSX); 
	yPerInch =	GetDeviceCaps(hdc, LOGPIXELSY); 

	ReleaseDC(m_hWnd, hdc);

	// At this point the border flag is set and so is the pixels per inch
	// so we can initalize the inset.
	SetDefaultInset();

	m_fInplaceActive = TRUE;

	// Create Text Services component
	m_pserv = GetTxtServiec(this);

	// Set window text
	if(pcs && pcs->lpszName)
	{	
		if(FAILED(m_pserv->TxSetText((TCHAR *)pcs->lpszName)))
			return FALSE;
	}

	m_rcClient.left = pcs->x;
	m_rcClient.top = pcs->y;
	m_rcClient.right = pcs->x + pcs->cx;
	m_rcClient.bottom = pcs->y + pcs->cy;

	// The extent matches the full client rectangle in HIMETRIC
#if 0
	m_sizelExtent.cx = DXtoHimetricX(pcs->cx - 2 * HOST_BORDER, xPerInch);
	m_sizelExtent.cy = DYtoHimetricY(pcs->cy - 2 * HOST_BORDER, yPerInch);
#endif

	// notify Text Services that we are in place active
	if(FAILED(m_pserv->OnTxInPlaceActivate(&m_rcClient)))
		return FALSE;
	
	// Hide all scrollbars to start
	if(m_hWnd && !(m_dwStyle & ES_DISABLENOSCROLL))
	{
		LONG m_dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		m_dwStyle &= ~(WS_VSCROLL | WS_HSCROLL);
		SetWindowLong(m_hWnd, GWL_STYLE, m_dwStyle);
	}

	if (!(m_dwStyle & ES_READONLY))
	{
		// This isn't a read only window so we need a drop target.
		RegisterDragDrop();
	}

	m_pserv->TxSendMessage(WM_SETFOCUS, NULL, NULL, NULL);

	return TRUE;
}


/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT RichEditHost::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) 
		|| IsEqualIID(riid, IID_ITextHost)) 
	{
		AddRef();
		*ppvObject = (ITextHost *) this;
		hr = S_OK;
	}

	return hr;
}

ULONG RichEditHost::AddRef(void)
{
	return ++m_cRefs;
}

ULONG RichEditHost::Release(void)
{
	ULONG c_Refs = --m_cRefs;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}


//////////////////////////////// Properties ////////////////////////////////


TXTEFFECT RichEditHost::TxGetEffects() const
{
	return (m_dwStyle & ES_SUNKEN) ? TXTEFFECT_SUNKEN : TXTEFFECT_NONE;
}


//////////////////////////// System API wrapper ////////////////////////////



///////////////////////  Windows message dispatch methods  ///////////////////////////////


void RichEditHost::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
	HWND hWnd = GetHWNDByNode(m_node);
	IntRect rtControl = m_node->BoundingRectAbsolute();

	HDC hdc = skia::BeginPlatformPaint(g->platformContext()->canvas());

	AffineTransform trans = g->getCTM();

	IntRect updateRect = rtControl;
	updateRect.intersect(rcInvalid);
	updateRect = trans.mapRect(updateRect);
	IntRect rtControlIntRect = trans.mapRect(rtControl);

	if (m_bIsTransparencyLayer) {
	} else {
		m_pserv->TxDraw(
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

// 	if (m_bCaretShow)
// 		g->drawRect(m_caretRect);

	skia::EndPlatformPaint(g->platformContext()->canvas());
}

LRESULT RichEditHost::Event(TEventUI* event) {
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
			m_pserv->TxSendMessage(message, 0, MAKELONG(pt.x, pt.y), &lResult);

		return lResult;
	} else if (0 != event->uMsg) {
		if (WM_SETCURSOR == event->uMsg)
			return lResult;

		// 处理输入法消息时，S_FALSE 表示要调用def函数，否则就不调用
		// Message was not processed by text services so send it
		// to the default window proc.
		event->bNotCallDefWindowProc = S_FALSE == m_pserv->TxSendMessage(event->uMsg, event->wParam, event->lParam, &lResult);
	}

	return lResult;
}

///////////////////////////////  Keyboard Messages  //////////////////////////////////


LRESULT RichEditHost::OnKeyDown(WORD vkey, DWORD dwFlags)
{
	switch(vkey)
	{
	case VK_ESCAPE:
		if(m_fInDialogBox)
		{
			PostMessage(m_hWndParent, WM_CLOSE, 0, 0);
			return 0;
		}
		break;
	
	case VK_RETURN:
		if(m_fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000) 
				&& !(m_dwStyle & ES_WANTRETURN))
		{
			// send to default button
			LRESULT id;
			HWND hwndT;

			id = SendMessage(m_hWndParent, DM_GETDEFID, 0, 0);
			if(LOWORD(id) &&
				(hwndT = GetDlgItem(m_hWndParent, LOWORD(id))))
			{
				SendMessage(m_hWndParent, WM_NEXTDLGCTL, (WPARAM) hwndT, (LPARAM) 1);
				if(GetFocus() != m_hWnd)
					PostMessage(hwndT, WM_KEYDOWN, (WPARAM) VK_RETURN, 0);
			}
			return 0;
		}
		break;

	case VK_TAB:
		if(m_fInDialogBox) 
		{
			SendMessage(m_hWndParent, WM_NEXTDLGCTL, 
							!!(GetKeyState(VK_SHIFT) & 0x8000), 0);
			return 0;
		}
		break;
	}
	return 1;
}

#define CTRL(_ch) (_ch - 'A' + 1)

LRESULT RichEditHost::OnChar(WORD vkey, DWORD dwFlags)
{
	switch(vkey)
	{
	// Ctrl-Return generates Ctrl-J (LF), treat it as an ordinary return
	case CTRL('J'):
	case VK_RETURN:
		if(m_fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000)
				 && !(m_dwStyle & ES_WANTRETURN))
			return 0;
		break;

	case VK_TAB:
		if(m_fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000))
			return 0;
	}
	
	return 1;
}


////////////////////////////////////  View rectangle //////////////////////////////////////

void RichEditHost::OnSetRect(LPRECT prc)
{
	RECT rcClient;
	
	if(!prc)
	{
		SetDefaultInset();
	}	
	else	
    {
    	// For screen display, the following intersects new view RECT
    	// with adjusted client area RECT
    	TxGetClientRect(&rcClient);

        // Adjust client rect
        // Factors in space for borders
        if(m_fBorder)
        {																					  
    	    rcClient.top		+= yHeightSys / 4;
    	    rcClient.bottom 	-= yHeightSys / 4 - 1;
    	    rcClient.left		+= xWidthSys / 2;
    	    rcClient.right	-= xWidthSys / 2;
        }
	
        // Ensure we have minimum width and height
        rcClient.right = max(rcClient.right, rcClient.left + xWidthSys);
        rcClient.bottom = max(rcClient.bottom, rcClient.top + yHeightSys);

        // Intersect the new view rectangle with the 
        // adjusted client area rectangle
        if(!IntersectRect(&rcViewInset, &rcClient, prc))
    	    rcViewInset = rcClient;

        // compute inset in pixels
        rcViewInset.left -= rcClient.left;
        rcViewInset.top -= rcClient.top;
        rcViewInset.right = rcClient.right - rcViewInset.right;
        rcViewInset.bottom = rcClient.bottom - rcViewInset.bottom;

		// Convert the inset to himetric that must be returned to ITextServices
        rcViewInset.left = DXtoHimetricX(rcViewInset.left, xPerInch);
        rcViewInset.top = DYtoHimetricY(rcViewInset.top, yPerInch);
        rcViewInset.right = DXtoHimetricX(rcViewInset.right, xPerInch);
        rcViewInset.bottom = DYtoHimetricY(rcViewInset.bottom, yPerInch);
    }

    m_pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, 
    	TXTBIT_VIEWINSETCHANGE);
}



////////////////////////////////////  System notifications  //////////////////////////////////


void RichEditHost::OnSysColorChange()
{
	crAutoColor = GetSysColor(COLOR_WINDOWTEXT);
	if(!m_fNotSysBkgnd)
		m_crBackground = GetSysColor(COLOR_WINDOW);
	TxInvalidateRect(NULL, TRUE);
}

LRESULT RichEditHost::OnGetDlgCode(WPARAM wparam, LPARAM lparam)
{
	LRESULT lres = DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;

	if(m_dwStyle & ES_MULTILINE)
		lres |= DLGC_WANTALLKEYS;

	if(!(m_dwStyle & ES_SAVESEL))
		lres |= DLGC_HASSETSEL;

	if(lparam)
		m_fInDialogBox = TRUE;

	if(lparam &&
		((WORD) wparam == VK_BACK))
	{
		lres |= DLGC_WANTMESSAGE;
	}

	return lres;
}


/////////////////////////////////  Other messages  //////////////////////////////////////


LRESULT RichEditHost::OnGetOptions() const
{
	LRESULT lres = (m_dwStyle & ECO_STYLES);

	if(m_fEnableAutoWordSel)
		lres |= ECO_AUTOWORDSELECTION;
	
	return lres;
}

void RichEditHost::OnSetOptions(WORD wOp, DWORD eco)
{
	const BOOL fAutoWordSel = !!(eco & ECO_AUTOWORDSELECTION);
	DWORD dwStyleNew = m_dwStyle;
	DWORD dw_Style = 0 ;

	DWORD dwChangeMask = 0;

	// single line controls can't have a selection bar
	// or do vertical writing
	if(!(dw_Style & ES_MULTILINE))
	{
#ifdef DBCS
		eco &= ~(ECO_SELECTIONBAR | ECO_VERTICAL);
#else
		eco &= ~ECO_SELECTIONBAR;
#endif
	}
	dw_Style = (eco & ECO_STYLES);

	switch(wOp)
	{
	case ECOOP_SET:
		dwStyleNew = ((dwStyleNew) & ~ECO_STYLES) | m_dwStyle;
		m_fEnableAutoWordSel = fAutoWordSel;
		break;

	case ECOOP_OR:
		dwStyleNew |= dw_Style;
		if(fAutoWordSel)
			m_fEnableAutoWordSel = TRUE;
		break;

	case ECOOP_AND:
		dwStyleNew &= (dw_Style | ~ECO_STYLES);
		if(m_fEnableAutoWordSel && !fAutoWordSel)
			m_fEnableAutoWordSel = FALSE;
		break;

	case ECOOP_XOR:
		dwStyleNew ^= dw_Style;
		m_fEnableAutoWordSel = (!m_fEnableAutoWordSel != !fAutoWordSel);
		break;
	}

	if(m_fEnableAutoWordSel != (unsigned)fAutoWordSel)
	{
		dwChangeMask |= TXTBIT_AUTOWORDSEL; 
	}

	if(dwStyleNew != dw_Style)
	{
		DWORD dwChange = dwStyleNew ^ dw_Style;
#ifdef DBCS
		USHORT	usMode;
#endif

		m_dwStyle = dwStyleNew;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyleNew);

		if(dwChange & ES_NOHIDESEL)	
		{
			dwChangeMask |= TXTBIT_HIDESELECTION;
		}

		if(dwChange & ES_READONLY)
		{
			dwChangeMask |= TXTBIT_READONLY;

			// Change drop target state as appropriate.
			if (dwStyleNew & ES_READONLY)
			{
				RevokeDragDrop();
			}
			else
			{
				RegisterDragDrop();
			}
		}

		if(dwChange & ES_VERTICAL)
		{
			dwChangeMask |= TXTBIT_VERTICAL;
		}

		// no action require for ES_WANTRETURN
		// no action require for ES_SAVESEL
		// do this last
		if(dwChange & ES_SELECTIONBAR)
		{
			m_lSelBarWidth = 212;
			dwChangeMask |= TXTBIT_SELBARCHANGE;
		}
	}

	if (dwChangeMask)
	{
		DWORD dwProp = 0;
		TxGetPropertyBits(dwChangeMask, &dwProp);
		m_pserv->OnTxPropertyBitsChange(dwChangeMask, dwProp);
	}
}

void RichEditHost::OnSetReadOnly(BOOL fReadOnly)
{
	DWORD dwUpdatedBits = 0;

	if(fReadOnly)
	{
		m_dwStyle |= ES_READONLY;

		// Turn off Drag Drop 
		RevokeDragDrop();
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		m_dwStyle &= ~(DWORD) ES_READONLY;

		// Turn drag drop back on
		RegisterDragDrop();	
	}

	m_pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
}

void RichEditHost::OnSetEventMask(DWORD mask)
{
	LRESULT lres = (LRESULT) m_dwEventMask;
	m_dwEventMask = (DWORD) mask;

}


LRESULT RichEditHost::OnGetEventMask() const
{
	return (LRESULT) m_dwEventMask;
}

/*
 *	RichEditHost::OnSetFont(hfont)
 *
 *	Purpose:	
 *		Set new font from hfont
 *
 *	Arguments:
 *		hfont	new font (NULL for system font)
 */
BOOL RichEditHost::OnSetFont(HFONT hfont)
{
	if(SUCCEEDED(InitDefaultCharFormat(&cf, hfont)))
	{
		m_pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
		return TRUE;
	}

	return FALSE;
}

/*
 *	RichEditHost::OnSetCharFormat(pcf)
 *
 *	Purpose:	
 *		Set new default CharFormat
 *
 *	Arguments:
 *		pch		ptr to new CHARFORMAT
 */
BOOL RichEditHost::OnSetCharFormat(CHARFORMAT *pcf)
{
	m_pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);

	return TRUE;
}

/*
 *	RichEditHost::OnSetParaFormat(ppf)
 *
 *	Purpose:	
 *		Set new default ParaFormat
 *
 *	Arguments:
 *		pch		ptr to new PARAFORMAT
 */
BOOL RichEditHost::OnSetParaFormat(PARAFORMAT *pPF)
{
	pf = *pPF;									// Copy it

	m_pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 
		TXTBIT_PARAFORMATCHANGE);

	return TRUE;
}



////////////////////////////  Event firing  /////////////////////////////////



void * RichEditHost::CreateNmhdr(UINT uiCode, LONG cb)
{
	NMHDR *pnmhdr;

	pnmhdr = (NMHDR*) new char[cb];
	if(!pnmhdr)
		return NULL;

	memset(pnmhdr, 0, cb);

	pnmhdr->hwndFrom = m_hWnd;
	pnmhdr->idFrom = GetWindowLong(m_hWnd, GWL_ID);
	pnmhdr->code = uiCode;

	return (VOID *) pnmhdr;
}


////////////////////////////////////  Helpers  /////////////////////////////////////////
void RichEditHost::SetDefaultInset()
{
    // Generate default view rect from client rect.
    if(m_fBorder)
    {
        // Factors in space for borders
  	    rcViewInset.top = DYtoHimetricY(yHeightSys / 4, yPerInch);
   	 rcViewInset.bottom	= DYtoHimetricY(yHeightSys / 4 - 1, yPerInch);
   	 rcViewInset.left = DXtoHimetricX(xWidthSys / 2, xPerInch);
   	 rcViewInset.right = DXtoHimetricX(xWidthSys / 2, xPerInch);
    }
    else
    {
		rcViewInset.top = rcViewInset.left =
		rcViewInset.bottom = rcViewInset.right = 0;
	}
}


/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC RichEditHost::TxImmGetContext(void)
{
	HIMC himc;
	himc = ImmGetContext( m_hWnd );
	return himc;
}

void RichEditHost::TxImmReleaseContext(HIMC himc)
{
	ImmReleaseContext( m_hWnd, himc );
}

void RichEditHost::RevokeDragDrop(void)
{
	if (m_fRegisteredForDrop)
	{
		::RevokeDragDrop(m_hWnd);
		m_fRegisteredForDrop = FALSE;
	}
}

void RichEditHost::RegisterDragDrop(void)
{
	IDropTarget *pdt;

	if(!m_fRegisteredForDrop && m_pserv->TxGetDropTarget(&pdt) == NOERROR)
	{
		HRESULT hr = ::RegisterDragDrop(m_hWnd, pdt);

		if(hr == NOERROR)
		{	
			m_fRegisteredForDrop = TRUE;
		}

		pdt->Release();
	}
}

#define cmultBorder 1

HRESULT RichEditHost::OnTxVisibleChange(BOOL fVisible)
{
	fVisible = fVisible;

	if(!fVisible && m_fResized)
	{
		RECT rc;
		// Control was resized while hidden,
		// need to really resize now
		TxGetClientRect(&rc);
		m_fResized = FALSE;
		m_pserv->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, 
			TXTBIT_CLIENTRECTCHANGE);
	}

	return S_OK;
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC RichEditHost::TxGetDC()
{
	return ::GetDC(m_hWnd);
}


int RichEditHost::TxReleaseDC(HDC hdc)
{
	return ::ReleaseDC (m_hWnd, hdc);
}


BOOL RichEditHost::TxShowScrollBar(INT fnBar,	BOOL fShow)
{
	return ::ShowScrollBar(m_hWnd, fnBar, fShow);
}

BOOL RichEditHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	return ::EnableScrollBar(m_hWnd, fuSBFlags, fuArrowflags) ;//SB_HORZ, ESB_DISABLE_BOTH);
}


BOOL RichEditHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	return ::SetScrollRange(m_hWnd, fnBar, nMinPos, nMaxPos, fRedraw);
}


BOOL RichEditHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
	return ::SetScrollPos(m_hWnd, fnBar, nPos, fRedraw);
}

void RichEditHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	InvalidateRect(prc, fMode, TRUE);
}

void RichEditHost::TxViewChange(BOOL fUpdate) 
{
	TxInvalidateRect(NULL, TRUE);
}

void RichEditHost::InvalidateRect(LPCRECT prc, BOOL fMode, BOOL bClicntRectDirty) {
	if (!m_bCanRepaint)
		return;

	if (bClicntRectDirty) // false == bClicntRectDirty 都是游标重绘导致的，为了不覆盖真正的重绘请求，故这里判断下
		m_bClicntRectDirty = TRUE;

	RECT rtUpdate;
	if(prc == NULL)
		GetControlRect(&rtUpdate);
	else
		rtUpdate = *prc;

	if (IntRect(rtUpdate).isEmpty())
		return;

	ASSERT(m_richEditNode != NULL);
	m_node->Invalidate();
}

BOOL RichEditHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	return ::CreateCaret (m_hWnd, hbmp, xWidth, yHeight);
}

BOOL RichEditHost::TxShowCaret(BOOL fShow)
{
	if(fShow)
		return ::ShowCaret(m_hWnd);
	else
		return ::HideCaret(m_hWnd);
}

BOOL RichEditHost::TxSetCaretPos(INT x, INT y)
{
	return ::SetCaretPos(x, y);
}

BOOL RichEditHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	m_fTimer = TRUE;
	return ::SetTimer(m_hWnd, idTimer, uTimeout, NULL);
}

void RichEditHost::TxKillTimer(UINT idTimer)
{
	::KillTimer(m_hWnd, idTimer);
	m_fTimer = FALSE;
}

void RichEditHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
	::ScrollWindowEx(m_hWnd, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate, fuScroll);
}

void RichEditHost::TxSetCapture(BOOL fCapture)
{
	if (fCapture)
		::SetCapture(m_hWnd);
	else
		::ReleaseCapture();
}

void RichEditHost::TxSetFocus()
{
	::SetFocus(m_hWnd);
}

void RichEditHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
	::SetCursor(hcur);
}

BOOL RichEditHost::TxScreenToClient(LPPOINT lppt)
{
	return ::ScreenToClient(m_hWnd, lppt);	
}

BOOL RichEditHost::TxClientToScreen(LPPOINT lppt)
{
	return ::ClientToScreen(m_hWnd, lppt);
}

HRESULT RichEditHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT RichEditHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}
    

HRESULT RichEditHost::TxGetClientRect(LPRECT prc)
{
	*prc = m_rcClient;

	GetControlRect(prc);

	return NOERROR;
}


HRESULT RichEditHost::TxGetViewInset(LPRECT prc) 
{

    *prc = rcViewInset;
    
    return NOERROR;	
}

HRESULT RichEditHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	*ppCF = &cf;
	return NOERROR;
}

HRESULT RichEditHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &pf;
	return NOERROR;
}


COLORREF RichEditHost::TxGetSysColor(int nIndex) 
{
	if (nIndex == COLOR_WINDOW)
	{
		if(!m_fNotSysBkgnd)
			return GetSysColor(COLOR_WINDOW);
		return m_crBackground;
	}

	return GetSysColor(nIndex);
}



HRESULT RichEditHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = !m_fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
	return NOERROR;
}


HRESULT RichEditHost::TxGetMaxLength(DWORD *pLength)
{
	*pLength = m_cchTextMost;
	return NOERROR;
}



HRESULT RichEditHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
						ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return NOERROR;
}


HRESULT RichEditHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef UNICODE
	*pch = chPasswordChar;
#else
	WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

HRESULT RichEditHost::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = laccelpos;
	return S_OK;
} 										   

HRESULT RichEditHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
	return S_OK;
}


HRESULT RichEditHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
	pf = *ppf;
	return S_OK;
}


HRESULT RichEditHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
	DWORD dwProperties = 0;

	if (m_fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

	if (m_dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (m_dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}


	if (m_dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(m_dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (m_fEnableAutoWordSel)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

	if (m_fVertical)
	{
		dwProperties |= TXTBIT_VERTICAL;
	}
					
	if (m_fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

	if (m_fAllowBeep)
	{
		dwProperties |= TXTBIT_ALLOWBEEP;
	}

	if (m_fSaveSelection)
	{
		dwProperties |= TXTBIT_SAVESELECTION;
	}

	*pdwBits = dwProperties & dwMask; 
	return NOERROR;
}


HRESULT RichEditHost::TxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE )
	{
		RECT rc;
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		
		GetControlRect(&rc);
		rc.bottom = rc.top + preqsz->rc.bottom + HOST_BORDER;
		rc.right  = rc.left + preqsz->rc.right + HOST_BORDER;
		rc.top -= HOST_BORDER;
		rc.left -= HOST_BORDER;
		
		SetClientRect(&rc, TRUE);
		
		return S_OK;
	} 

	// Forward this to the container
	if (pnc)
	{
		(*pnc)(iNotify);
	}

	return S_OK;
}

// 计算放大倍数用。
HRESULT RichEditHost::TxGetExtent(LPSIZEL lpExtent)
{
	// Calculate the length & convert to himetric
	*lpExtent = m_sizelExtent;

	return S_OK;
}

void RichEditHost::SetExtent(SIZEL *psizelExtent, BOOL fNotify) 
{ 
	// Set our extent
	m_sizelExtent = *psizelExtent; 

	// Notify the host that the extent has changed
	if (fNotify)
		m_pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);

}

HRESULT	RichEditHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
	*plSelBarWidth = m_lSelBarWidth;
	return S_OK;
}


BOOL RichEditHost::GetReadOnly()
{
	return (m_dwStyle & ES_READONLY) != 0;
}

void RichEditHost::SetReadOnly(BOOL fReadOnly)
{
	if (fReadOnly)
	{
		m_dwStyle |= ES_READONLY;
	}
	else
	{
		m_dwStyle &= ~ES_READONLY;
	}

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
		fReadOnly ? TXTBIT_READONLY : 0);
}

BOOL RichEditHost::GetAllowBeep()
{
	return m_fAllowBeep;
}

void RichEditHost::SetAllowBeep(BOOL m_fAllowBeep)
{
	m_fAllowBeep = m_fAllowBeep;

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
		m_fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

void RichEditHost::SetViewInset(RECT *prc)
{
	rcViewInset = *prc;

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, 0);
}

WORD RichEditHost::GetDefaultAlign()
{
	return pf.wAlignment;
}


void RichEditHost::SetDefaultAlign(WORD wNewAlign)
{
	pf.wAlignment = wNewAlign;

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL RichEditHost::GetRichTextFlag()
{
	return m_fRich;
}

void RichEditHost::SetRichTextFlag(BOOL fNew)
{
	m_fRich = fNew;

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
		fNew ? TXTBIT_RICHTEXT : 0);
}

LONG RichEditHost::GetDefaultLeftIndent()
{
	return pf.dxOffset;
}


void RichEditHost::SetDefaultLeftIndent(LONG lNewIndent)
{
	pf.dxOffset = lNewIndent;

	// Notify control of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void RichEditHost::SetClientRect(RECT *prc, BOOL fUpdateExtent) 
{
	// If the extent matches the client rect then we assume the extent should follow
	// the client rect.
	LONG lTestExt = DYtoHimetricY(
		(m_rcClient.bottom - m_rcClient.top)  - 2 * HOST_BORDER, yPerInch);
#if 0
	if (fUpdateExtent && (m_sizelExtent.cy == lTestExt))
	{
		m_sizelExtent.cy = DXtoHimetricX((prc->bottom - prc->top) - 2 * HOST_BORDER, 
			xPerInch);
		m_sizelExtent.cx = DYtoHimetricY((prc->right - prc->left) - 2 * HOST_BORDER,
			yPerInch);
	}
#endif
	m_rcClient = *prc; 
}

BOOL RichEditHost::SetSaveSelection(BOOL f_SaveSelection)
{
	BOOL fResult = f_SaveSelection;

	m_fSaveSelection = f_SaveSelection;

	// notify text services of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
		m_fSaveSelection ? TXTBIT_SAVESELECTION : 0);

	return fResult;		
}

HRESULT	RichEditHost::OnTxInPlaceDeactivate()
{
	HRESULT hr = m_pserv->OnTxInPlaceDeactivate();

	if (SUCCEEDED(hr))
	{
		m_fInplaceActive = FALSE;
	}

	return hr;
}

HRESULT	RichEditHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
	m_fInplaceActive = TRUE;

	HRESULT hr = m_pserv->OnTxInPlaceActivate(prcClient);

	if (FAILED(hr))
	{
		m_fInplaceActive = FALSE;
	}

	return hr;
}

BOOL RichEditHost::DoSetCursor(RECT *prc, POINT *pt)
{
	RECT rc = prc ? *prc : m_rcClient;

	// Give some space for our border
	rc.top += HOST_BORDER;
	rc.bottom -= HOST_BORDER;
	rc.left += HOST_BORDER;
	rc.right -= HOST_BORDER;

	// Is this in our rectangle?
	if (PtInRect(&rc, *pt))
	{
		RECT *prcClient = (!m_fInplaceActive || prc) ? &rc : NULL;

		HDC hdc = GetDC(m_hWnd);

		m_pserv->OnTxSetCursor(
			DVASPECT_CONTENT,	
			-1,
			NULL,
			NULL,
			hdc,
			NULL,
			prcClient,
			pt->x, 
			pt->y);

		ReleaseDC(m_hWnd, hdc);

		return TRUE;
	}

	return FALSE;
}

void RichEditHost::GetControlRect(
	LPRECT prc			//@parm	Where to put client coordinates
)
{
	IntRect rc = m_node->BoundingRectAbsolute();
	m_rcClient = (RECT)rc;

	// Give some space for our border
// 	prc->top = m_rcClient.top + HOST_BORDER;
// 	prc->bottom = m_rcClient.bottom - HOST_BORDER;
// 	prc->left = m_rcClient.left + HOST_BORDER;
// 	prc->right = m_rcClient.right - HOST_BORDER;
}

void RichEditHost::SetTransparent(BOOL f_Transparent)
{
	m_fTransparent = f_Transparent;

	// notify text services of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG RichEditHost::SetAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	laccelpos = l_accelpos;

	// notify text services of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

	return laccelposOld;
}

WCHAR RichEditHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
	WCHAR chOldPasswordChar = chPasswordChar;

	chPasswordChar = ch_PasswordChar;

	// notify text services of property change
	m_pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
		(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

	return chOldPasswordChar;
}

void RichEditHost::SetDisabled(BOOL fOn)
{
	cf.dwMask	  |= CFM_COLOR	   | CFM_DISABLED;
	cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

	if( !fOn )
	{
		cf.dwEffects &= ~CFE_DISABLED;
	}
	
	m_pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

LONG RichEditHost::SetSelBarWidth(LONG l_SelBarWidth)
{
	LONG lOldSelBarWidth = m_lSelBarWidth;

	m_lSelBarWidth = l_SelBarWidth;

	if (m_lSelBarWidth)
	{
		m_dwStyle |= ES_SELECTIONBAR;
	}
	else
	{
		m_dwStyle &= (~ES_SELECTIONBAR);
	}

	m_pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

	return lOldSelBarWidth;
}

BOOL RichEditHost::GetTimerState()
{
	return m_fTimer;
}