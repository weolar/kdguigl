
#include <windows.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x502
#endif

// #ifdef UILIB_EXPORTS
// #define UILIB_API __declspec(dllexport)
// #else
// #define UILIB_API __declspec(dllimport)
// #endif

// #include <Gdiplus.h>
// #pragma comment (lib, "GdiPlus.lib")

#if _MSC_VER > 1310
#include <Gdiplus.h>
#pragma comment (lib, "GdiPlus.lib")
#else
#include "gdiplus/Gdiplus.h"
#pragma comment (lib, "kdgui/gdiplus/Gdiplus.lib")
#endif

#define _USER_SCRIPT_  

using namespace Gdiplus;

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )

#ifdef UILIB_IMPORT
#define UILIB_API __declspec(dllimport)
#else
#define UILIB_API 
#endif

#define lengthof(x) (sizeof(x)/sizeof(*x))

#ifndef _INC_WINDOWSX

#define SNDMSG ::SendMessage

#define SetWindowFont(hwnd, hfont, fRedraw) FORWARD_WM_SETFONT((hwnd), (hfont), (fRedraw), SNDMSG)

#define HANDLE_WM_SETFONT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HFONT)(wParam), (BOOL)(lParam)), 0L)

#define FORWARD_WM_SETFONT(hwnd, hfont, fRedraw, fn) \
    (void)(fn)((hwnd), WM_SETFONT, (WPARAM)(HFONT)(hfont), (LPARAM)(BOOL)(fRedraw))

#define Static_Enable(hwndCtl, fEnable)         EnableWindow((hwndCtl), (fEnable))

#define Static_GetText(hwndCtl, lpch, cchMax)   GetWindowText((hwndCtl), (lpch), (cchMax))
#define Static_GetTextLength(hwndCtl)           GetWindowTextLength(hwndCtl)
#define Static_SetText(hwndCtl, lpsz)           SetWindowText((hwndCtl), (lpsz))

#define Static_SetIcon(hwndCtl, hIcon)          ((HICON)(UINT_PTR)SNDMSG((hwndCtl), STM_SETICON, (WPARAM)(HICON)(hIcon), 0L))
#define Static_GetIcon(hwndCtl, hIcon)          ((HICON)(UINT_PTR)SNDMSG((hwndCtl), STM_GETICON, 0L, 0L))

/****** Button control message APIs ******************************************/

#define Button_Enable(hwndCtl, fEnable)         EnableWindow((hwndCtl), (fEnable))

#define Button_GetText(hwndCtl, lpch, cchMax)   GetWindowText((hwndCtl), (lpch), (cchMax))
#define Button_GetTextLength(hwndCtl)           GetWindowTextLength(hwndCtl)
#define Button_SetText(hwndCtl, lpsz)           SetWindowText((hwndCtl), (lpsz))

#define Button_GetCheck(hwndCtl)            ((int)(DWORD)SNDMSG((hwndCtl), BM_GETCHECK, 0L, 0L))
#define Button_SetCheck(hwndCtl, check)     ((void)SNDMSG((hwndCtl), BM_SETCHECK, (WPARAM)(int)(check), 0L))

#define Button_GetState(hwndCtl)            ((int)(DWORD)SNDMSG((hwndCtl), BM_GETSTATE, 0L, 0L))
#define Button_SetState(hwndCtl, state)     ((UINT)(DWORD)SNDMSG((hwndCtl), BM_SETSTATE, (WPARAM)(int)(state), 0L))

#define Button_SetStyle(hwndCtl, style, fRedraw) ((void)SNDMSG((hwndCtl), BM_SETSTYLE, (WPARAM)LOWORD(style), MAKELPARAM(((fRedraw) ? TRUE : FALSE), 0)))

/****** Edit control message APIs ********************************************/

#define Edit_Enable(hwndCtl, fEnable)           ::EnableWindow((hwndCtl), (fEnable))

#define Edit_GetText(hwndCtl, lpch, cchMax)     ::GetWindowText((hwndCtl), (lpch), (cchMax))
#define Edit_GetTextLength(hwndCtl)             ::GetWindowTextLength(hwndCtl)
#define Edit_SetText(hwndCtl, lpsz)             ::SetWindowText((hwndCtl), (lpsz))

#define Edit_LimitText(hwndCtl, cchMax)         ((void)SNDMSG((hwndCtl), EM_LIMITTEXT, (WPARAM)(cchMax), 0L))

#define Edit_GetLineCount(hwndCtl)              ((int)(DWORD)SNDMSG((hwndCtl), EM_GETLINECOUNT, 0L, 0L))
#ifndef _MAC
#define Edit_GetLine(hwndCtl, line, lpch, cchMax) ((*((int *)(lpch)) = (cchMax)), ((int)(DWORD)SNDMSG((hwndCtl), EM_GETLINE, (WPARAM)(int)(line), (LPARAM)(LPTSTR)(lpch))))
#else
#define Edit_GetLine(hwndCtl, line, lpch, cchMax) ((*((WORD *)(lpch)) = (cchMax)), ((int)(DWORD)SNDMSG((hwndCtl), EM_GETLINE, (WPARAM)(int)(line), (LPARAM)(LPTSTR)(lpch))))
#endif

#define Edit_GetRect(hwndCtl, lprc)             ((void)SNDMSG((hwndCtl), EM_GETRECT, 0L, (LPARAM)(RECT *)(lprc)))
#define Edit_SetRect(hwndCtl, lprc)             ((void)SNDMSG((hwndCtl), EM_SETRECT, 0L, (LPARAM)(const RECT *)(lprc)))
#define Edit_SetRectNoPaint(hwndCtl, lprc)      ((void)SNDMSG((hwndCtl), EM_SETRECTNP, 0L, (LPARAM)(const RECT *)(lprc)))

#define Edit_GetSel(hwndCtl)                    ((DWORD)SNDMSG((hwndCtl), EM_GETSEL, 0L, 0L))
#define Edit_SetSel(hwndCtl, ichStart, ichEnd)  ((void)SNDMSG((hwndCtl), EM_SETSEL, (ichStart), (ichEnd)))
#define Edit_ReplaceSel(hwndCtl, lpszReplace)   ((void)SNDMSG((hwndCtl), EM_REPLACESEL, 0L, (LPARAM)(LPCTSTR)(lpszReplace)))

#define Edit_GetModify(hwndCtl)                 ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_GETMODIFY, 0L, 0L))
#define Edit_SetModify(hwndCtl, fModified)      ((void)SNDMSG((hwndCtl), EM_SETMODIFY, (WPARAM)(UINT)(fModified), 0L))

#define Edit_ScrollCaret(hwndCtl)               ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_SCROLLCARET, 0, 0L))

#define Edit_LineFromChar(hwndCtl, ich)         ((int)(DWORD)SNDMSG((hwndCtl), EM_LINEFROMCHAR, (WPARAM)(int)(ich), 0L))
#define Edit_LineIndex(hwndCtl, line)           ((int)(DWORD)SNDMSG((hwndCtl), EM_LINEINDEX, (WPARAM)(int)(line), 0L))
#define Edit_LineLength(hwndCtl, line)          ((int)(DWORD)SNDMSG((hwndCtl), EM_LINELENGTH, (WPARAM)(int)(line), 0L))

#define Edit_Scroll(hwndCtl, dv, dh)            ((void)SNDMSG((hwndCtl), EM_LINESCROLL, (WPARAM)(dh), (LPARAM)(dv)))

#define Edit_CanUndo(hwndCtl)                   ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_CANUNDO, 0L, 0L))
#define Edit_Undo(hwndCtl)                      ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_UNDO, 0L, 0L))
#define Edit_EmptyUndoBuffer(hwndCtl)           ((void)SNDMSG((hwndCtl), EM_EMPTYUNDOBUFFER, 0L, 0L))

#define Edit_SetPasswordChar(hwndCtl, ch)       ((void)SNDMSG((hwndCtl), EM_SETPASSWORDCHAR, (WPARAM)(UINT)(ch), 0L))

#define Edit_SetTabStops(hwndCtl, cTabs, lpTabs) ((void)SNDMSG((hwndCtl), EM_SETTABSTOPS, (WPARAM)(int)(cTabs), (LPARAM)(const int *)(lpTabs)))

#define Edit_FmtLines(hwndCtl, fAddEOL)         ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_FMTLINES, (WPARAM)(BOOL)(fAddEOL), 0L))

#define Edit_GetHandle(hwndCtl)                 ((HLOCAL)(UINT_PTR)SNDMSG((hwndCtl), EM_GETHANDLE, 0L, 0L))
#define Edit_SetHandle(hwndCtl, h)              ((void)SNDMSG((hwndCtl), EM_SETHANDLE, (WPARAM)(UINT_PTR)(HLOCAL)(h), 0L))

#define Edit_GetFirstVisibleLine(hwndCtl)       ((int)(DWORD)SNDMSG((hwndCtl), EM_GETFIRSTVISIBLELINE, 0L, 0L))

#define Edit_SetReadOnly(hwndCtl, fReadOnly)    ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_SETREADONLY, (WPARAM)(BOOL)(fReadOnly), 0L))

#define Edit_GetPasswordChar(hwndCtl)           ((TCHAR)(DWORD)SNDMSG((hwndCtl), EM_GETPASSWORDCHAR, 0L, 0L))

#define Edit_SetWordBreakProc(hwndCtl, lpfnWordBreak) ((void)SNDMSG((hwndCtl), EM_SETWORDBREAKPROC, 0L, (LPARAM)(EDITWORDBREAKPROC)(lpfnWordBreak)))
#define Edit_GetWordBreakProc(hwndCtl)          ((EDITWORDBREAKPROC)SNDMSG((hwndCtl), EM_GETWORDBREAKPROC, 0L, 0L))


#define GET_WPARAM(wp, lp)                      (wp)
#define GET_LPARAM(wp, lp)                      (lp)

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define GET_WM_ACTIVATE_STATE(wp, lp)           LOWORD(wp)
#define GET_WM_ACTIVATE_FMINIMIZED(wp, lp)      (BOOL)HIWORD(wp)
#define GET_WM_ACTIVATE_HWND(wp, lp)            (HWND)(lp)
#define GET_WM_ACTIVATE_MPS(s, fmin, hwnd)   \
    (WPARAM)MAKELONG((s), (fmin)), (LPARAM)(hwnd)

#define GET_WM_CHARTOITEM_CHAR(wp, lp)          (TCHAR)LOWORD(wp)
#define GET_WM_CHARTOITEM_POS(wp, lp)           HIWORD(wp)
#define GET_WM_CHARTOITEM_HWND(wp, lp)          (HWND)(lp)
#define GET_WM_CHARTOITEM_MPS(ch, pos, hwnd) \
    (WPARAM)MAKELONG((pos), (ch)), (LPARAM)(hwnd)

#define GET_WM_COMMAND_ID(wp, lp)               LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp)             (HWND)(lp)
#define GET_WM_COMMAND_CMD(wp, lp)              HIWORD(wp)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd)    \
    (WPARAM)MAKELONG(id, cmd), (LPARAM)(hwnd)

#define WM_CTLCOLOR                             0x0019

#define GET_WM_CTLCOLOR_HDC(wp, lp, msg)        (HDC)(wp)
#define GET_WM_CTLCOLOR_HWND(wp, lp, msg)       (HWND)(lp)
#define GET_WM_CTLCOLOR_TYPE(wp, lp, msg)       (WORD)(msg - WM_CTLCOLORMSGBOX)
#define GET_WM_CTLCOLOR_MSG(type)               (WORD)(WM_CTLCOLORMSGBOX+(type))
#define GET_WM_CTLCOLOR_MPS(hdc, hwnd, type) \
    (WPARAM)(hdc), (LPARAM)(hwnd)


#define GET_WM_MENUSELECT_CMD(wp, lp)               LOWORD(wp)
#define GET_WM_MENUSELECT_FLAGS(wp, lp)             (UINT)(int)(short)HIWORD(wp)
#define GET_WM_MENUSELECT_HMENU(wp, lp)             (HMENU)(lp)
#define GET_WM_MENUSELECT_MPS(cmd, f, hmenu)  \
    (WPARAM)MAKELONG(cmd, f), (LPARAM)(hmenu)

/* Note: the following are for interpreting MDIclient to MDI child messages. */
#define GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wp, lp)  (lp == (LPARAM)hwnd)
#define GET_WM_MDIACTIVATE_HWNDDEACT(wp, lp)        (HWND)(wp)
#define GET_WM_MDIACTIVATE_HWNDACTIVATE(wp, lp)     (HWND)(lp)
/* Note: the following is for sending to the MDI client window. */
#define GET_WM_MDIACTIVATE_MPS(f, hwndD, hwndA)\
    (WPARAM)(hwndA), 0

#define GET_WM_MDISETMENU_MPS(hmenuF, hmenuW) (WPARAM)hmenuF, (LPARAM)hmenuW

#define GET_WM_MENUCHAR_CHAR(wp, lp)                (TCHAR)LOWORD(wp)
#define GET_WM_MENUCHAR_HMENU(wp, lp)               (HMENU)(lp)
#define GET_WM_MENUCHAR_FMENU(wp, lp)               (BOOL)HIWORD(wp)
#define GET_WM_MENUCHAR_MPS(ch, hmenu, f)    \
    (WPARAM)MAKELONG(ch, f), (LPARAM)(hmenu)

#define GET_WM_PARENTNOTIFY_MSG(wp, lp)             LOWORD(wp)
#define GET_WM_PARENTNOTIFY_ID(wp, lp)              HIWORD(wp)
#define GET_WM_PARENTNOTIFY_HWNDCHILD(wp, lp)       (HWND)(lp)
#define GET_WM_PARENTNOTIFY_X(wp, lp)               (int)(short)LOWORD(lp)
#define GET_WM_PARENTNOTIFY_Y(wp, lp)               (int)(short)HIWORD(lp)
#define GET_WM_PARENTNOTIFY_MPS(msg, id, hwnd) \
    (WPARAM)MAKELONG(id, msg), (LPARAM)(hwnd)
#define GET_WM_PARENTNOTIFY2_MPS(msg, x, y) \
    (WPARAM)MAKELONG(0, msg), MAKELONG(x, y)

#define GET_WM_VKEYTOITEM_CODE(wp, lp)              (int)(short)LOWORD(wp)
#define GET_WM_VKEYTOITEM_ITEM(wp, lp)              HIWORD(wp)
#define GET_WM_VKEYTOITEM_HWND(wp, lp)              (HWND)(lp)
#define GET_WM_VKEYTOITEM_MPS(code, item, hwnd) \
    (WPARAM)MAKELONG(item, code), (LPARAM)(hwnd)

#define GET_EM_SETSEL_START(wp, lp)                 (INT)(wp)
#define GET_EM_SETSEL_END(wp, lp)                   (lp)
#define GET_EM_SETSEL_MPS(iStart, iEnd) \
    (WPARAM)(iStart), (LPARAM)(iEnd)

#define GET_EM_LINESCROLL_MPS(vert, horz)     \
    (WPARAM)horz, (LPARAM)vert

#define GET_WM_CHANGECBCHAIN_HWNDNEXT(wp, lp)       (HWND)(lp)

#define GET_WM_HSCROLL_CODE(wp, lp)                 LOWORD(wp)
#define GET_WM_HSCROLL_POS(wp, lp)                  HIWORD(wp)
#define GET_WM_HSCROLL_HWND(wp, lp)                 (HWND)(lp)
#define GET_WM_HSCROLL_MPS(code, pos, hwnd)    \
    (WPARAM)MAKELONG(code, pos), (LPARAM)(hwnd)

#define GET_WM_VSCROLL_CODE(wp, lp)                 LOWORD(wp)
#define GET_WM_VSCROLL_POS(wp, lp)                  HIWORD(wp)
#define GET_WM_VSCROLL_HWND(wp, lp)                 (HWND)(lp)
#define GET_WM_VSCROLL_MPS(code, pos, hwnd)    \
    (WPARAM)MAKELONG(code, pos), (LPARAM)(hwnd)

#define GetWindowStyle(hwnd) ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define MapWindowRect(hwndFrom, hwndTo, lprc) MapWindowPoints((hwndFrom), (hwndTo), (POINT *)(lprc), 2)

#define KdGuiSubclassWindow(hwnd, lpfn) ((WNDPROC)SetWindowLong((hwnd), GWL_WNDPROC, (LPARAM)(WNDPROC)(lpfn)))
#define GetWindowOwner(hwnd) GetWindow(hwnd, GW_OWNER)
#define GetWindowStyle(hwnd) ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define GetWindowExStyle(hwnd) ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))


#endif // end _INC_WINDOWSX
