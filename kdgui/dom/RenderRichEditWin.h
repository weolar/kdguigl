/*
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.
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

#ifndef RenderRichEditWin_h
#define RenderRichEditWin_h

#include <windows.h>
#include <richedit.h>
#include <TextServ.h>
#include <WTF/HashMap.h>

#include "RichEditBaseNode.h"

//namespace WebCore {

const LONG cInitTextMax = (32 * 1024) - 1;

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

class RenderRichEdit;
class RenderRichEditTimer;
struct RenderRichEditTimeData;

struct RenderSVGRichEditImplScrollData {
    RenderSVGRichEditImplScrollData() {
        nMinPos = 0;
        nMaxPos = 0;
        nPos = 0;
        nWidth = 0;
        nHeight = 0;
        bShow = FALSE;
    }

    int nMinPos;
    int nMaxPos;
    int nPos;
    int nWidth;
    int nHeight;
    BOOL bShow;
};

class OffScreenDC;

class RenderRichEdit : public ITextHost {
public:
    RenderRichEdit(RichEditBaseNode *richEditNode);
    BOOL Init(const CREATESTRUCT *pcs);
    VOID UnInit();
    BOOL HasInit() {return m_pTextService != NULL;}
    RichEditBaseNode* GetNode() { return m_richEditNode; }

    void ref() { m_nRef++; }
    void deref() { m_nRef--; }

    virtual ~RenderRichEdit();

    void SetOrKillFocus(BOOL bSet);

    void SetCanRepaint(BOOL bCanRepaint) { m_bCanRepaint = bCanRepaint; }
    void InvalidateRect(LPCRECT prc, BOOL fMode, BOOL bClicntRectDirty);

    void Paint(GraphicsContext* g, const IntRect &rcInvalid);
    LRESULT Event(TEventUI* event);

    ITextServices* GetTextServices(void) { return m_pTextService ;}

    VOID GetViewInset(RECT& rt) const;
    VOID SetViewInset(const RECT& rt);

    void SetClientRect(const IntRect& prc);
    const IntRect& GetClientRect() { return m_rcClient; }
    BOOL GetWordWrap(void) { return m_fWordWrap; }
    void SetWordWrap(BOOL fWordWrap);
    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);
    void SetFont(HFONT hFont);
    void SetColor(COLORREF clrText);
    SIZEL* GetExtent();
    void SetExtent(SIZEL *psizelExtent);
    void LimitText(LONG nChars);
    LONG GetLimitText();
    BOOL IsCaptured();

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);
    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);
    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);
    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);
    BOOL SetSaveSelection(BOOL fSaveSelection);
    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
    BOOL GetActiveState(void) { return m_fInplaceActive; }
    BOOL DoSetCursor(RECT *prc, POINT *pt);
    void SetTransparent(BOOL fTransparent);
    void GetControlRect(IntRect& prc);
    void GetLocalControlRect(IntRect& prc);
    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);
    BOOL GetTimerState();
    VOID SetEventMask(DWORD dwMask);
    DWORD GetEventMask() const;

    void SetCharFormat(CHARFORMAT2W &c);
    void SetParaFormat(PARAFORMAT2 &p);

    void CaretFire(Timer<RenderRichEdit>*);

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------
    virtual HDC TxGetDC();
    virtual INT TxReleaseDC(HDC hdc);
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
    virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL TxShowCaret(BOOL fShow);
    virtual BOOL TxSetCaretPos(INT x, INT y);
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL TxScreenToClient (LPPOINT lppt);
    virtual BOOL TxClientToScreen (LPPOINT lppt);
    virtual HRESULT TxActivate( LONG * plOldState );
    virtual HRESULT TxDeactivate( LONG lNewState );
    virtual HRESULT TxGetClientRect(LPRECT prc);
    virtual HRESULT TxGetViewInset(LPRECT prc);
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    virtual COLORREF TxGetSysColor(int nIndex);
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
    virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
    virtual HIMC TxImmGetContext(void);
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

    RenderSVGRichEditImplScrollData m_VScrollData;
    RenderSVGRichEditImplScrollData m_HScrollData;

protected:
    BOOL InitDefaultCharFormat();
    BOOL InitDefaultParaFormat();

	void DrawLayer(GraphicsContext* g, HDC hdc, 
		const IntRect& rtControl, const IntRect& rtControlIntRect, const IntRect& updateRect);

private:
	void RichEditTimeMsgDispatch(RenderRichEditTimer*);
	BOOL CreateSysCaret(HBITMAP hbmp, INT xWidth, INT yHeight);

    int m_nRef;

    BOOL m_bCanRepaint;
    RichEditBaseNode* m_richEditNode;
    ITextServices* m_pTextService;		    // pointer to Text Services object

    BOOL m_bCaretShow;
    BOOL m_bClicntRectDirty; // 调用了TxInvalidateRect之类的后，需要重新刷新脏矩形
    IntRect m_caretRect;
    OffScreenDC* m_offScreen;
    BOOL m_bIsTransparencyLayer;
    BOOL m_bHaveMove; // 鼠标第一次移动后，重置这个标志

    // Properties
    DWORD   m_dwStyle;				// style bits

    BOOL	m_fEnableAutoWordSel;	// enable Word style auto word selection?
    BOOL	m_fWordWrap;	        // Whether control should word wrap
    BOOL	m_fAllowBeep;	        // Whether beep is allowed
    BOOL	m_fRich;	            // Whether control is rich text
    BOOL	m_fSaveSelection;	    // Whether to save the selection when inactive
    BOOL	m_fInplaceActive;       // Whether control is inplace active
    BOOL	m_fTransparent;         // Whether control is transparent
    BOOL	m_fTimer;	            // A timer is set
    BOOL    m_fCaptured;

    LONG		m_lSelBarWidth;			// Width of the selection bar
    LONG  		m_cchTextMost;			// maximum text size
    DWORD		m_dwEventMask;			// DoEvent mask to pass on to parent window
    LONG		m_icf;
    LONG		m_ipf;
    IntRect		m_rcClient;				// Client Rect for this control
    SIZEL		m_sizelExtent;			// Extent array
    CHARFORMAT2W m_cf;					// Default character format
    PARAFORMAT2	m_pf;					    // Default paragraph format
    LONG		m_laccelpos;				// Accelerator position
    WCHAR		m_chPasswordChar;		    // Password character
    RECT		m_rtInset;
    RECT		m_rtInsetHimetric;

	WTF::HashMap<UINT, RenderRichEditTimeData*> m_timers;
    //Timer*    m_time;

    Timer<RenderRichEdit>*  m_caretTimer;
    ULONGLONG m_lastMouseUpTime;
	bool m_bCreateSysCaret;

    static const int m_scrollWidth = 10;  // 竖滚动条的宽度
    static const int m_scrollHeight = 10; // 横滚动条的高度
};


//} // namespace WebCore

#endif // RenderRichEditWin_h