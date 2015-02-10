/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/



/**************************************************************************
 HOST.H  Text Host for Window's Rich Edit Control 
**************************************************************************/


#ifndef RichEditHost_H
#define RichEditHost_H


#include "textserv.h"

class RichEditBaseNode;
class GraphicsContext;
struct TEventUI;

#define FValidCF(_pcf) ((_pcf)->cbSize == sizeof(CHARFORMAT))
#define FValidPF(_ppf) ((_ppf)->cbSize == sizeof(PARAFORMAT))

typedef void (* PNOTIFY_CALL)(int iNotify);

#define HOST_BORDER 5

#define LY_PER_INCH   1440

EXTERN_C const IID IID_ITextEditControl;

/**************************************************************************
 	TXTEFFECT
 
 	@enum	Defines different background styles control
**************************************************************************/
enum TXTEFFECT {
	TXTEFFECT_NONE = 0,				//@emem	no special backgoround effect
	TXTEFFECT_SUNKEN,				//@emem	draw a "sunken 3-D" look
};

/**************************************************************************
 	RichEditHost

 	@class	Text Host for Window's Rich Edit Control implementation class
/**************************************************************************/
class RichEditHost : public ITextHost
{
public:

	RichEditHost(RichEditBaseNode* pEdit);

	// Initialization
	BOOL Init(HWND h_wnd, const CREATESTRUCT *pcs, PNOTIFY_CALL p_nc);
	BOOL HasInit() {return m_pserv != NULL;}

	~RichEditHost();

	ITextServices * GetTextServices(void) { return m_pserv; }

	void Paint(GraphicsContext* g, const IntRect &rcInvalid);
	LRESULT Event(TEventUI* event);
	void SetCanRepaint(bool bCanRepaint) { m_bCanRepaint = bCanRepaint; }

	void SetClientRect(RECT *prc, BOOL fUpdateExtent = TRUE);

	RECT * GetClientRect() { return &m_rcClient; }

	void SetExtent(SIZEL *psizelExtent, BOOL fNotify);

	PNOTIFY_CALL SetNotifyCall(PNOTIFY_CALL p_nc)
	{
	    PNOTIFY_CALL pncRet = pnc;
		pnc = p_nc;
		return pncRet;
	}

	BOOL GetWordWrap(void) { return m_fWordWrap; }

	void SetWordWrap(BOOL m_fWordWrap)
	{
	    m_fWordWrap = m_fWordWrap;
		m_pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, m_fWordWrap ? TXTBIT_WORDWRAP : 0);
    }

	BOOL GetReadOnly();

	void SetReadOnly(BOOL fReadOnly);

	BOOL GetAllowBeep();

	void SetAllowBeep(BOOL m_fAllowBeep);

	void SetViewInset(RECT *prc);

	WORD GetDefaultAlign();

	void SetDefaultAlign(WORD wNewAlign);

	BOOL GetRichTextFlag();

	void SetRichTextFlag(BOOL fNew);

	LONG GetDefaultLeftIndent();

	void SetDefaultLeftIndent(LONG lNewIndent);

	BOOL SetSaveSelection(BOOL m_fSaveSelection);

	SIZEL *GetExtent(void) { return &m_sizelExtent; }

	HRESULT OnTxInPlaceDeactivate();

	HRESULT OnTxInPlaceActivate(LPCRECT prcClient);

	BOOL GetActiveState(void) { return m_fInplaceActive; }

	BOOL DoSetCursor(RECT *prc, POINT *pt);

	void SetTransparent(BOOL m_fTransparent);

	void GetControlRect(LPRECT prc);

	LONG SetAccelPos(LONG laccelpos);

	WCHAR SetPasswordChar(WCHAR chPasswordChar);

	void SetDisabled(BOOL fOn);

	LONG SetSelBarWidth(LONG m_lSelBarWidth);

	BOOL GetTimerState();

	static LONG GetXPerInch(void) { return xPerInch; }

	static LONG	GetYPerInch(void) { return yPerInch; }


	// -----------------------------
	//	IUnknown interface
	// -----------------------------

	virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG _stdcall AddRef(void);
	virtual ULONG _stdcall Release(void);

	// -----------------------------
	//	ITextHost interface
	// -----------------------------
	//@cmember Get the DC for the host
	virtual HDC TxGetDC();

	//@cmember Release the DC gotten from the host
	virtual INT TxReleaseDC(HDC hdc);
	
	//@cmember Show the scroll bar
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);

	//@cmember Enable the scroll bar
	virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);

	//@cmember Set the scroll range
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);

	//@cmember Set the scroll position
	virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);

	//@cmember InvalidateRect
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);

	//@cmember Send a WM_PAINT to the window
	virtual void TxViewChange(BOOL fUpdate);
	
	//@cmember Create the caret
	virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);

	//@cmember Show the caret
	virtual BOOL TxShowCaret(BOOL fShow);

	//@cmember Set the caret position
	virtual BOOL TxSetCaretPos(INT x, INT y);

	//@cmember Create a timer with the specified timeout
	virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);

	//@cmember Destroy a timer
	virtual void TxKillTimer(UINT idTimer);

	//@cmember Scroll the content of the specified window's client area
	virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	
	//@cmember Get mouse capture
	virtual void TxSetCapture(BOOL fCapture);

	//@cmember Set the focus to the text window
	virtual void TxSetFocus();

	//@cmember Establish a new cursor shape
	virtual void TxSetCursor(HCURSOR hcur, BOOL fText);

	//@cmember Converts screen coordinates of a specified point to the client coordinates 
	virtual BOOL TxScreenToClient (LPPOINT lppt);

	//@cmember Converts the client coordinates of a specified point to screen coordinates
	virtual BOOL TxClientToScreen (LPPOINT lppt);

	//@cmember Request host to activate text services
	virtual HRESULT TxActivate( LONG * plOldState );

	//@cmember Request host to deactivate text services
   	virtual HRESULT TxDeactivate( LONG lNewState );

	//@cmember Retrieves the coordinates of a window's client area
	virtual HRESULT TxGetClientRect(LPRECT prc);

	//@cmember Get the view rectangle relative to the inset
	virtual HRESULT TxGetViewInset(LPRECT prc);

	//@cmember Get the default character format for the text
	virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );

	//@cmember Get the default paragraph format for the text
	virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);

	//@cmember Get the background color for the window
	virtual COLORREF TxGetSysColor(int nIndex);

	//@cmember Get the background (either opaque or transparent)
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);

	//@cmember Get the maximum length for the text
	virtual HRESULT TxGetMaxLength(DWORD *plength);

	//@cmember Get the bits representing requested scroll bars for the window
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);

	//@cmember Get the character to display for password input
	virtual HRESULT TxGetPasswordChar(TCHAR *pch);

	//@cmember Get the accelerator character
	virtual HRESULT TxGetAcceleratorPos(LONG *pcp);

	//@cmember Get the native size
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);

	//@cmember Notify host that default character format has changed
	virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);

	//@cmember Notify host that default paragraph format has changed
	virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);

	//@cmember Bulk access to bit properties
	virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);

	//@cmember Notify host of events
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);

	//@cmember Get the undo manager for the host

	// FE Support Routines for handling the Input Method Context
	virtual HIMC TxImmGetContext(void);
	virtual void TxImmReleaseContext(HIMC himc);

	//@cmember Returns HIMETRIC size of the control bar.
	virtual HRESULT TxGetSelectionBarWidth (LONG *m_lSelBarWidth);

private:

	void InvalidateRect(LPCRECT prc, BOOL fMode, BOOL bClicntRectDirty);


	// helpers
	void *CreateNmhdr(UINT uiCode, LONG cb);
	void	RevokeDragDrop(void);
	void	RegisterDragDrop(void);
	void	DrawSunkenBorder(HWND hwnd, HDC hdc);
	VOID  OnSunkenWindowPosChanging(HWND hwnd, WINDOWPOS *pwndpos);
	LRESULT OnSize(HWND hwnd, WORD fwSizeType, int nWidth, int nHeight);
	TXTEFFECT TxGetEffects() const;
	HRESULT	OnTxVisibleChange(BOOL m_fVisible);
	void	SetDefaultInset();

	// Keyboard messages
	LRESULT	OnKeyDown(WORD vKey, DWORD dwFlags);
	LRESULT	OnChar(WORD vKey, DWORD dwFlags);
	
	// System notifications
	void 	OnSysColorChange();
	LRESULT OnGetDlgCode(WPARAM wparam, LPARAM lparam);

	// Other messages
	LRESULT OnGetOptions() const;
	void	OnSetOptions(WORD wOp, DWORD eco);
	LRESULT	OnGetEventMask() const;
	void    OnSetEventMask(DWORD dwMask);
	void	OnSetReadOnly(BOOL fReadOnly);
	BOOL	OnSetFont(HFONT hFont);
	BOOL	OnSetCharFormat(CHARFORMAT *pcf);
	BOOL	OnSetParaFormat(PARAFORMAT *ppf);
	void	OnGetRect(LPRECT prc);
	void	OnSetRect(LPRECT prc);

private:
	RichEditBaseNode* m_node;
	bool m_bIsTransparencyLayer;
	BOOL m_bHaveMove;
	DWORD m_lastMouseUpTime;
	bool m_bCanRepaint;

	BOOL m_bClicntRectDirty; // 调用了TxInvalidateRect之类的后，需要重新刷新脏矩形

    static LONG xWidthSys;
    static LONG yHeightSys;
	static LONG xPerInch;
	static LONG yPerInch;

	PNOTIFY_CALL pnc;				// Notify container of the control of changes

	ULONG	m_cRefs;					// Reference Count

	HWND	m_hWnd;					// control window
	HWND	m_hWndParent;			// parent window

	ITextServices	*m_pserv;		// pointer to Text Services object

// Properties

	DWORD		m_dwStyle;				// style bits
	DWORD		dwExStyle;				// extended style bits

	unsigned	m_fBorder			:1;	// control has border
	unsigned	m_fCustRect			:1;	// client changed format rect
	unsigned	m_fInBottomless		:1;	// inside bottomless callback
	unsigned	m_fInDialogBox		:1;	// control is in a dialog box
	unsigned	m_fEnableAutoWordSel:1;	// enable Word style auto word selection?
	unsigned	m_fVertical			:1;	// vertical writing
	unsigned	m_fIconic			:1;	// control window is iconic
	unsigned	m_fHidden			    :1;	// control window is hidden
	unsigned	m_fNotSysBkgnd		:1;	// not using system background color
	unsigned	m_fWindowLocked		:1;	// window is locked (no update)
	unsigned	m_fRegisteredForDrop	:1; // whether host has registered for drop
	unsigned	m_fVisible			:1;	// Whether window is visible or not.
	unsigned	m_fResized			:1;	// resized while hidden
	unsigned	m_fWordWrap			:1;	// Whether control should word wrap
	unsigned	m_fAllowBeep			:1;	// Whether beep is allowed
	unsigned	m_fRich				:1;	// Whether control is rich text
	unsigned	m_fSaveSelection		:1;	// Whether to save the selection when inactive
	unsigned	m_fInplaceActive		:1; // Whether control is inplace active
	unsigned	m_fTransparent		:1; // Whether control is transparent
	unsigned	m_fTimer				:1;	// A timer is set

	LONG		m_lSelBarWidth;			// Width of the selection bar

	COLORREF 	m_crBackground;			// background color
	LONG  		m_cchTextMost;			// maximum text size
	DWORD		   m_dwEventMask;			// Event mask to pass on to parent window

	LONG		m_icf;
	LONG		m_ipf;

	RECT		m_rcClient;				// Client Rect for this control

   RECT     rcViewInset;           // view rect inset 

	SIZEL		m_sizelExtent;			// Extent array

	CHARFORMAT2W	cf;					// Default character format

	PARAFORMAT	pf;					// Default paragraph format

	LONG		laccelpos;				// Accelerator position

	WCHAR		chPasswordChar;		// Password character	
};


#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
						ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
						ECO_SELECTIONBAR)

// Window creation/destruction
HRESULT CreateHost(HWND hwnd, const CREATESTRUCT *pcs, PNOTIFY_CALL pnotify, RichEditHost **pptwh);


#endif // RichEditHost_H
