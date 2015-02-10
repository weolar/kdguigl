
#include "StdAfx.h"
#include <stdio.h>
#include <tchar.h>
#include "scriptapi.h"
#include "script/scripthelp/SquirrelBindingsUtils.h"
#include "UIAnim.h"

//////////////////////////////////////////////////////////////////////////
// 脚本里消息常量

// struct ScriptClassMemberDecl __KdMsg_members[] = 
// { 
// 	{NULL,NULL,NULL,NULL}
// };
// struct ScriptConstantDecl __KdMsg_constants[] = 
// { 
// 	{_T("UIEVENT__FIRST"),OT_INTEGER,UIEVENT__FIRST},
// 	{NULL,OT_NULL,0}
// };
// struct ScriptNamespaceDecl __KdMsg_decl = 
// {
// 	_T("KdMsg"), __KdMsg_members,__KdMsg_constants,NULL 
// };

_BEGIN_NAMESPACE(KdMsg)
_BEGIN_NAMESPACE_CONSTANTS(KdMsg)
_CONSTANT(UIEVENT__FIRST,OT_INTEGER,UIEVENT__FIRST)
_CONSTANT(UIEVENT_MOUSEMOVE,OT_INTEGER,UIEVENT_MOUSEMOVE)
_CONSTANT(UIEVENT_MOUSELEAVE,OT_INTEGER,UIEVENT_MOUSELEAVE)
_CONSTANT(UIEVENT_MOUSEENTER,OT_INTEGER,UIEVENT_MOUSEENTER)
_CONSTANT(UIEVENT_MOUSEHOVER,OT_INTEGER,UIEVENT_MOUSEHOVER)
_CONSTANT(UIEVENT_KEYDOWN,OT_INTEGER,UIEVENT_KEYDOWN)
_CONSTANT(UIEVENT_KEYUP,OT_INTEGER,UIEVENT_KEYUP)
_CONSTANT(UIEVENT_CHAR,OT_INTEGER,UIEVENT_CHAR)
_CONSTANT(UIEVENT_SYSKEY,OT_INTEGER,UIEVENT_SYSKEY)
_CONSTANT(UIEVENT_SETFOCUS,OT_INTEGER,UIEVENT_SETFOCUS)
_CONSTANT(UIEVENT_BUTTONDOWN,OT_INTEGER,UIEVENT_BUTTONDOWN)
_CONSTANT(UIEVENT_BUTTONUP,OT_INTEGER,UIEVENT_BUTTONUP)
_CONSTANT(UIEVENT_DBLCLICK,OT_INTEGER,UIEVENT_DBLCLICK)
_CONSTANT(UIEVENT_CONTEXTMENU,OT_INTEGER,UIEVENT_CONTEXTMENU)
_CONSTANT(UIEVENT_VSCROLL,OT_INTEGER,UIEVENT_VSCROLL)
_CONSTANT(UIEVENT_HSCROLL,OT_INTEGER,UIEVENT_HSCROLL)
_CONSTANT(UIEVENT_SCROLLWHEEL,OT_INTEGER,UIEVENT_SCROLLWHEEL)
_CONSTANT(UIEVENT_WINDOWSIZE,OT_INTEGER,UIEVENT_WINDOWSIZE)
_CONSTANT(UIEVENT_SETCURSOR,OT_INTEGER,UIEVENT_SETCURSOR)
_CONSTANT(UIEVENT_MEASUREITEM,OT_INTEGER,UIEVENT_MEASUREITEM)
_CONSTANT(UIEVENT_DRAWITEM,OT_INTEGER,UIEVENT_DRAWITEM)
_CONSTANT(UIEVENT_TIMER,OT_INTEGER,UIEVENT_TIMER)
_CONSTANT(UIEVENT_NOTIFY,OT_INTEGER,UIEVENT_NOTIFY)
_CONSTANT(UIEVENT_COMMAND,OT_INTEGER,UIEVENT_COMMAND)
_CONSTANT(UIEVENT_RBUTTONDOWN,OT_INTEGER,UIEVENT_RBUTTONDOWN)
_CONSTANT(UIEVENT__LAST,OT_INTEGER,UIEVENT__LAST)
_END_NAMESPACE(KdMsg,NULL)
//////////////////////////////////////////////////////////////////////////
// 窗口类常量

_BEGIN_NAMESPACE(KdVm)
_BEGIN_NAMESPACE_CONSTANTS(KdVm)

_CONSTANT(HWND_TOP,OT_INTEGER,(int)HWND_TOP)
_CONSTANT(HWND_BOTTOM,OT_INTEGER,(int)HWND_BOTTOM)
_CONSTANT(HWND_TOPMOST,OT_INTEGER,(int)HWND_TOPMOST)
_CONSTANT(HWND_NOTOPMOST,OT_INTEGER,(int)HWND_NOTOPMOST)

_CONSTANT(SWP_NOSIZE,OT_INTEGER,SWP_NOSIZE)
_CONSTANT(SWP_NOMOVE,OT_INTEGER,SWP_NOMOVE)
_CONSTANT(SWP_NOZORDER,OT_INTEGER,SWP_NOZORDER)
_CONSTANT(SWP_NOREDRAW,OT_INTEGER,SWP_NOREDRAW)
_CONSTANT(SWP_NOACTIVATE,OT_INTEGER,SWP_NOACTIVATE)
_CONSTANT(SWP_FRAMECHANGED,OT_INTEGER,SWP_FRAMECHANGED)
_CONSTANT(SWP_SHOWWINDOW,OT_INTEGER,SWP_SHOWWINDOW)
_CONSTANT(SWP_HIDEWINDOW,OT_INTEGER,SWP_HIDEWINDOW)
_CONSTANT(SWP_NOCOPYBITS,OT_INTEGER,SWP_NOCOPYBITS)
_CONSTANT(SWP_NOOWNERZORDER,OT_INTEGER,SWP_NOOWNERZORDER)
_CONSTANT(SWP_NOSENDCHANGING,OT_INTEGER,SWP_NOSENDCHANGING)
_CONSTANT(SWP_DRAWFRAME,OT_INTEGER,SWP_DRAWFRAME)
_CONSTANT(SWP_NOREPOSITION,OT_INTEGER,SWP_NOREPOSITION)
_CONSTANT(SWP_DEFERERASE,OT_INTEGER,SWP_DEFERERASE)
_CONSTANT(SWP_ASYNCWINDOWPOS,OT_INTEGER,SWP_ASYNCWINDOWPOS)

_CONSTANT(LWA_COLORKEY,OT_INTEGER,LWA_COLORKEY)
_CONSTANT(LWA_ALPHA,OT_INTEGER,LWA_ALPHA)
_END_NAMESPACE(KdVm,NULL)


_BEGIN_NAMESPACE(KdTwTransition)
_BEGIN_NAMESPACE_CONSTANTS(KdTwTransition)
_CONSTANT(SINE, OT_INTEGER, SINE)
_CONSTANT(QUINT, OT_INTEGER, QUINT)
_CONSTANT(QUART, OT_INTEGER, QUART)
_CONSTANT(QUAD, OT_INTEGER, QUAD)
_CONSTANT(EXPO, OT_INTEGER, EXPO)
_CONSTANT(ELASTIC, OT_INTEGER, ELASTIC)
_CONSTANT(CUBIC, OT_INTEGER, CUBIC)
_CONSTANT(CIRC, OT_INTEGER, CIRC)
_CONSTANT(BOUNCE, OT_INTEGER, BOUNCE)
_CONSTANT(BACK, OT_INTEGER, BACK)
_END_NAMESPACE(KdTwTransition,NULL)


_BEGIN_NAMESPACE(KdTwEquation)
_BEGIN_NAMESPACE_CONSTANTS(KdTwEquation)
_CONSTANT(EASE_IN, OT_INTEGER, EASE_IN)
_CONSTANT(EASE_OUT, OT_INTEGER, EASE_OUT)
_CONSTANT(EASE_IN_OUT, OT_INTEGER, EASE_IN_OUT)
_END_NAMESPACE(KdTwEquation,NULL)
//////////////////////////////////////////////////////////////////////////
// 脚本里需要用到的API


void QiTestArgs(HSQUIRRELVM v, int n)
{
	switch(sq_gettype(v,n))
	{
	case OT_NULL:
		printf("null");        
		break;
	case OT_INTEGER:
		printf("integer");
		break;
	case OT_FLOAT:
		printf("float");
		break;
	case OT_STRING:
		printf("string");
		break;    
	case OT_TABLE:
		printf("table");
		break;
	case OT_ARRAY:
		printf("array");
		break;
	case OT_USERDATA:
		printf("userdata");
		break;
	case OT_CLOSURE:        
		printf("closure(function)");    
		break;
	case OT_NATIVECLOSURE:
		printf("native closure(C function)");
		break;
	case OT_GENERATOR:
		printf("generator");
		break;
	case OT_USERPOINTER:
		printf("userpointer");
		break;
	case OT_CLASS:
		printf("class");
		break;
	case OT_INSTANCE:
		printf("instance");
		break;
	case OT_WEAKREF:
		printf("weak reference");
		break;
	default:
		sq_throwerror(v,L"invalid param"); //throws an exception
	}
		printf("\n");
}

// 脚本里注册某个控件的系统消息回调
// 脚本调用原型：
// BOOL ViRegSysMsgCallBack(VApiHandle hWnd, int nMsg, string Func);
// 参数2为控件接收到的外部系统消息.其实还有控件内部的消息,这里不能注册,但能被接受到
// Func 的C原型为typedef int (FN_QiCallVmMsg)(HANDLE hHandle, int nMsg, int nSubMsg, int x, int y, int Key, SQInteger nOtherInfo, SQChar* sOtherInfo);
SQInteger ViRegSysMsgCallBack(HSQUIRRELVM v)
{
	BOOL           bRet          = FALSE;
	VApiHandle     hWnd          = NULL;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nargs         = sq_gettop(v);
	int            Msg           = NULL;
	const SQChar*  pFunc         = NULL;
	PVOID          pMsgVmCallArr = NULL;

	ASSERT(nargs == 3 + 1 && v);
	if (!v || 3 + 1 != nargs) {bRet = FALSE; goto _Exit_;}

	QiTestArgs(v, 2);
	QiTestArgs(v, 3);
	QiTestArgs(v, 4);
	if (OT_INTEGER != sq_gettype(v, 2) ||
		OT_INTEGER != sq_gettype(v, 3) ||
		OT_STRING  != sq_gettype(v, 4))
	{bRet = FALSE; goto _Exit_;}

	sq_getinteger(v, 2, &hWnd);
	pCtrl = QiHwHandleToCtrl(hWnd); // 这里没验证控件指针的有效性，以后有时间再补上
	ASSERT(pCtrl);
	if (!pCtrl) {bRet = FALSE; goto _Exit_;}

	sq_getinteger(v, 3, &Msg);
	sq_getstring(v, 4, &pFunc);
	if (!pFunc || QiScriptMsgNameMaxLen < wcslen(pFunc)) {bRet = FALSE; goto _Exit_;}

    pCtrl->RegSysMsgToScript(Msg, pFunc);
	 
	bRet = TRUE;
	
_Exit_:
	sq_pushbool(v, bRet); 
	return 1;
}

// 脚本里注册某个控件的控件消息回调
// 脚本调用原型：
// BOOL ViRegCtrlMsgCallBack(VApiHandle hWnd, string Msg, string Func);
// 参数2为控件接收到的外部系统消息.其实还有控件内部的消息,这里不能注册,但能被接受到
// Func 的C原型为typedef int (FN_QiCallVmMsg)(HANDLE hHandle, int nMsg, int nSubMsg, int x, int y, int Key, SQInteger nOtherInfo, SQChar* sOtherInfo);
SQInteger ViRegCtrlMsgCallBack(HSQUIRRELVM v)
{
	BOOL           bRet          = FALSE;
	VApiHandle     hWnd          = NULL;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nargs         = sq_gettop(v);
	const SQChar*  Msg           = NULL;
	const SQChar*  pFunc         = NULL;
	PVOID          pMsgVmCallArr = NULL;

	ASSERT(nargs == 3 + 1 && v);
	if (!v || 3 + 1 != nargs) {bRet = FALSE; goto _Exit_;}

	QiTestArgs(v, 2);
	QiTestArgs(v, 3);
	QiTestArgs(v, 4);
	if (OT_INTEGER != sq_gettype(v, 2) ||
		OT_STRING  != sq_gettype(v, 3) ||
		OT_STRING  != sq_gettype(v, 4))
	{bRet = FALSE; goto _Exit_;}

	sq_getinteger(v, 2, &hWnd);
	pCtrl = QiHwHandleToCtrl(hWnd); // 这里没验证控件指针的有效性，以后有时间再补上
	ASSERT(pCtrl);
	if (!pCtrl) {bRet = FALSE; goto _Exit_;}

	sq_getstring(v, 3, &Msg);
	sq_getstring(v, 4, &pFunc);
	if (!pFunc || QiScriptMsgNameMaxLen < wcslen(pFunc)) {bRet = FALSE; goto _Exit_;}

	pCtrl->RegCtrlMsgToScript(Msg, pFunc);

	bRet = TRUE;

_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

// vm readme:
// 脚本调用到C的函数里时，总是会多一个table的参数，而且是第一个参数
// OT_CLOSURE类型表示脚本里的函数

// 脚本调用原型：
// VApiHandle VApiHwFindControl(HANDLE hWnd, string CtrlName);
SQInteger ViHwFindControl(HSQUIRRELVM v)
{
	VApiHandle       hRet           = 0;
	const SQChar*    szCtrlName    = NULL;
	SQInteger        nargs         = sq_gettop(v);
    SQInteger        nWnd          = 0;
    HANDLE           hWnd          = NULL;
	CScriptMgr*      pMgr          = NULL;
    CPaintManagerUI* pPM           = NULL;

	if (!v || 2 + 1 != nargs) {goto _Exit_;}

	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_STRING != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &nWnd);
    sq_getstring(v, 3, &szCtrlName);

    if (-1 == nWnd) {
        pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        pPM = pMgr->GetManager();
    } else {
        pPM = QiHwHandleToWin(nWnd)->pWinObj->GetPM();
    }
    if (!pPM) {goto _Exit_;}

	// fixbug - 未验证pm合法性
	hRet = QiHwCtrlToHandle(pPM->FindControl(szCtrlName));

_Exit_:
	sq_pushinteger(v, hRet);
	return 1;
}

// BOOL ViMessageBox(string sText)
SQInteger ViMessageBox(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	LPCTSTR        sText      = NULL;
	if (!v || 1 + 1 != nargs) {return 0;}
	if (OT_STRING != sq_gettype(v, 2)) {return 0;}

	sq_getstring(v, 2, &sText);
	//MessageBox(0, sText, 0, 0);
	OutputDebugStringW(sText);

	sq_pushbool(v, TRUE);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 开始实现脚本对控件操作的api

// HANDLE ViCbCreateCtrl(string sCtrlClass)
SQInteger ViCbCreateCtrl(HSQUIRRELVM v)
{
    SQInteger        nargs         = sq_gettop(v);
    SQInteger        hRet          = NULL;
    CControlUI*      pCtrl         = NULL;
    const SQChar*    sCtrlClass    = NULL;

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_STRING != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getstring(v, 2, &sCtrlClass);

    pCtrl = CDialogBuilder::RetCtrlByClass((LPCTSTR)sCtrlClass);
    if (!pCtrl) {goto _Exit_;}
    
    hRet = QiHwCtrlToHandle(pCtrl);

_Exit_:
    sq_pushinteger(v, hRet);
    return 1;
}

// BOOL ViCbAddCtrl(HANDLE hCtrl, HANDLE hParent)
SQInteger ViCbAddCtrl(HSQUIRRELVM v)
{
    SQInteger        nargs         = sq_gettop(v);
    SQInteger        hCtrl         = 0;
    SQInteger        hParent       = 0;
    CContainerUI*    pParentCtrl   = NULL;
    CScriptMgr*      pMgr          = NULL;
    CPaintManagerUI* pPM           = NULL;
    CControlUI*      pCtrl         = NULL;
    SQBool           bRet          = FALSE;

    if (!v || 2 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}

    sq_getinteger(v, 2, &hCtrl);
    sq_getinteger(v, 3, &hParent);

    pCtrl = QiHwHandleToCtrl(hCtrl);
    pParentCtrl = (CContainerUI*)QiHwHandleToCtrl(hParent);
    if (!pCtrl) {goto _Exit_;}
    if (NULL == pParentCtrl) {
        pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        pParentCtrl = (CContainerUI*)pPM->GetRoot();
    }
    if (!pParentCtrl || !pParentCtrl->IsContainer()) {goto _Exit_;}
    bRet = (SQBool)pParentCtrl->Add(pCtrl);

_Exit_:
    sq_pushbool(v, bRet);
    return 1;
}

// wchar* ViCbGetName(VApiHandle hWnd)
SQInteger ViCbGetName(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	CStdString     sName;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	sName = pCtrl->GetName();

_Exit_:
	sq_pushstring(v, sName.GetData(), sName.GetLength());
	return 1;
}

//   BOOL ViCbSetName(VApiHandle hWnd, LPCTSTR pstrName)
SQInteger ViCbSetName(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	LPCTSTR        pstrName      = NULL;
	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_STRING != sq_gettype(v, 3)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	sq_getstring(v, 3, &pstrName);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetName(pstrName);
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

// VApiHandle ViCbGetParent(VApiHandle hWnd)
SQInteger ViCbGetParent(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	CControlUI*    pParent       = NULL;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pParent = pCtrl->GetParent();
	
_Exit_:
	sq_pushinteger(v, (SQInteger)QiHwCtrlToHandle(pParent));
	return 1;
}

// wchar* ViCbGetText(VApiHandle hWnd)
SQInteger ViCbGetText(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	CStdString     sName;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	sName = pCtrl->GetText();
	
_Exit_:
	sq_pushstring(v, sName.GetData(), sName.GetLength());
	return 1;
}

//   BOOL ViCbSetText(VApiHandle hWnd, LPCTSTR pstrName)
SQInteger ViCbSetText(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	LPCTSTR        pstrName      = NULL;
	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_STRING != sq_gettype(v, 3)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	sq_getstring(v, 3, &pstrName);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetText(pstrName);
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

// wchar* ViCbGetToolTip(VApiHandle hWnd)
SQInteger ViCbGetToolTip(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	CStdString     sName;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	sName = pCtrl->GetToolTip();
	
_Exit_:
	sq_pushstring(v, sName.GetData(), sName.GetLength());
	return 1;
}

//   BOOL ViCbSetToolTip(VApiHandle hWnd, LPCTSTR pstrName)
SQInteger ViCbSetToolTip(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	LPCTSTR        pstrName      = NULL;
	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_STRING != sq_gettype(v, 3)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	sq_getstring(v, 3, &pstrName);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetToolTip(pstrName);
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//   BOOL ViCbSetFocus(VApiHandle hWnd)
SQInteger ViCbSetFocus(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	LPCTSTR        pstrName      = NULL;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetFocus();
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//   bool ViCbIsVisible(VApiHandle hWnd)
SQInteger ViCbIsVisible(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQBool         bRet          = FALSE;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	bRet = pCtrl->IsVisible();
	
_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

//   bool ViCbIsEnabled(VApiHandle hWnd)
SQInteger ViCbIsEnabled(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQBool         bRet          = FALSE;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	bRet = pCtrl->IsEnabled();
	
_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

//   bool ViCbIsFocused(VApiHandle hWnd)
SQInteger ViCbIsFocused(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQBool         bRet          = FALSE;
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	bRet = pCtrl->IsFocused();
	
_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

//   int  ViCbGetPosX(VApiHandle hWnd)
SQInteger ViCbGetPosX(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nRet          = 0;
	RECT           rc            = {0};
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	rc = pCtrl->GetPos();
	nRet = rc.left;
	
_Exit_:
	sq_pushinteger(v, nRet);
	return 1;
}

//   int  ViCbGetPosY(VApiHandle hWnd)
SQInteger ViCbGetPosY(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nRet          = 0;
	RECT           rc            = {0};
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	rc = pCtrl->GetPos();
	nRet = rc.top;
	
_Exit_:
	sq_pushinteger(v, nRet);
	return 1;
}

//   int  ViCbGetPosWight(VApiHandle hWnd)
SQInteger ViCbGetPosWight(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nRet          = 0;
	RECT           rc            = {0};
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	rc = pCtrl->GetPos();
	nRet = rc.right - rc.left;
	
_Exit_:
	sq_pushinteger(v, nRet);
	return 1;
}

//   int  ViCbGetPosHight(VApiHandle hWnd)
SQInteger ViCbGetPosHight(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nRet          = 0;
	RECT           rc            = {0};
	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	rc = pCtrl->GetPos();
	nRet = rc.bottom - rc.top;
	
_Exit_:
	sq_pushinteger(v, nRet);
	return 1;
}

//  bool  ViCbSetiPos(VApiHandle hWnd, int x, int y, int nWight, int nHight)
SQInteger ViCbSetiPos(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	int            x             = 0;
	int            y             = 0;
	int            nWight        = 0;
	int            nHight        = 0;
	RECT           rc            = {0};
	if (!v || 5 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 3) && OT_FLOAT  != sq_gettype(v, 3)) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 4) && OT_FLOAT  != sq_gettype(v, 4)) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 5) && OT_FLOAT  != sq_gettype(v, 5)) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 6) && OT_FLOAT  != sq_gettype(v, 6)) {goto _Exit_;}
	
	
	sq_getinteger(v, 2, &Handle);
	sq_getinteger(v, 3, &x);
	sq_getinteger(v, 4, &y);
	sq_getinteger(v, 5, &nWight);
	sq_getinteger(v, 6, &nHight);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}

	rc.left = x;
	rc.top = y;
	rc.right = x + nWight;
	rc.bottom = y + nHight;
	pCtrl->SetPos(rc);
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//  BOOL  ViCbSetPos(VApiHandle hWnd, int x0, int y0, int x1, int y1)
SQInteger ViCbSetPos(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	int            x0            = 0;
	int            y0            = 0;
	int            x1            = 0;
	int            y1            = 0;
	RECT           rc            = {0};
	if (!v || 5 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 4)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 5)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 6)) {goto _Exit_;}
	
	sq_getinteger(v, 2, &Handle);
	sq_getinteger(v, 3, &x0);
	sq_getinteger(v, 4, &y0);
	sq_getinteger(v, 5, &x1);
	sq_getinteger(v, 6, &y1);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}

	rc.left = x0;
	rc.top = y0;
	rc.right = x1;
	rc.bottom = y1;
	pCtrl->SetPos(rc);
	
_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//  BOOL  ViCbSetZrol(VApiHandle hWnd, int nZrol)
SQInteger ViCbSetZrol(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      nZrol        = 0;

	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3) && OT_FLOAT != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &Handle);
	sq_getinteger(v, 3, &nZrol);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetZrol(nZrol);

_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//  BOOL  ViCbSetAlpha(VApiHandle hWnd, BYTE cAlpha)
SQInteger ViCbSetAlpha(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	SQInteger      cAlpha        = 0;

	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3) && OT_FLOAT != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &Handle);
	sq_getinteger(v, 3, &cAlpha);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}
	pCtrl->SetAlpha((BYTE)cAlpha);

_Exit_:
	sq_pushbool(v, TRUE);
	return 1;
}

//  BOOL  ViCbApplyAttributeList(VApiHandle hWnd, LPCTSTR pstrList)
SQInteger ViCbApplyAttributeList(HSQUIRRELVM v)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	CControlUI*    pCtrl         = NULL;
	LPCTSTR        pstrList      = NULL;
	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_STRING  != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &Handle);
	sq_getstring(v, 3, &pstrList);
	pCtrl = QiHwHandleToCtrl(Handle);
	if (!pCtrl) {goto _Exit_;}

	pCtrl->ApplyAttributeList(pstrList);

_Exit_:
	sq_pushbool(v, TRUE);
	return 1;

}

//////////////////////////////////////////////////////////////////////////
//
// Qi前缀代表脚本接口，表示脚本API与控件粘合层中的子层，
// 其实还是为了实现脚本API单独出来的层
// Hw表示句柄管理
// 本函数将脚本传来的句柄转换为控件UI对象
CControlUI* QiHwHandleToCtrl(int Handle)
{
	return (CControlUI*)Handle;
}

WinMgrItem* QiHwHandleToWin(int Handle)
{
    return (WinMgrItem*)Handle;
}

VApiHandle QiHwCtrlToHandle(CControlUI* pCtrl)
{
	return (VApiHandle)pCtrl;
}

VApiHandle QiHwWinToHandle(WinMgrItem* pCtrl)
{
    return (VApiHandle)pCtrl;
}

VApiHandle QiHwObjToHandle(PVOID pObj)
{
	return (VApiHandle)pObj;
}

PVOID QiHwHandleToObj(VApiHandle hObj)
{
	return (PVOID)hObj;
}

// Vw表示窗口相关的工具函数
VOID __stdcall _VwSetTimeoutFuncInternal(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime, BOOL bIsTimeout)
{
	SetTimerParam* pParam = (SetTimerParam*)idEvent;
	SQInteger top = sq_gettop(pParam->v);
	sq_pushroottable(pParam->v);
	sq_pushstring(pParam->v, pParam->sFunc.GetData(), -1); // 脚本里的消息回调
	if(SQ_FAILED(sq_get(pParam->v, -2))) { 
		return;
	}

	sq_pushroottable(pParam->v);
	sq_pushobject(pParam->v, pParam->Obj);

	SQRESULT nVmRet = sq_call(pParam->v, 2, SQTrue, SQFalse);
	if (bIsTimeout) {::KillTimer(hWnd, idEvent);}
	if (SQ_FAILED(nVmRet)) {
		return;
	}

	return;
}


VOID __stdcall _VwSetTimerFunc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    _VwSetTimeoutFuncInternal(hWnd, uMsg, idEvent, dwTime, FALSE);
}

VOID __stdcall _VwSetTimeoutFunc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	_VwSetTimeoutFuncInternal(hWnd, uMsg, idEvent, dwTime, TRUE);
}

SQInteger _VwSetTimerInternal(HSQUIRRELVM v, TIMERPROC lpTimerFunc)
{
	SQInteger      nargs         = sq_gettop(v);
	SQInteger      Handle        = 0;
	SQInteger      uElapse       = 0;
	LPCTSTR        pstrFunc      = NULL;
	SetTimerParam* pTimerParam   = NULL;
	SQUserPointer      nRet          = NULL;

	if (!v || 3 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_STRING   != sq_gettype(v, 3)) {goto _Exit_;}
	if (OT_INSTANCE != sq_gettype(v, 4)) {goto _Exit_;}

	sq_getinteger(v, 2, &uElapse);
	sq_getstring(v, 3, &pstrFunc);

	pTimerParam = new SetTimerParam();
	pTimerParam->v = v;
	pTimerParam->sFunc = pstrFunc;
	sq_getstackobj(v, 4, &pTimerParam->Obj);

	CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
	if (!pMgr) {goto _Exit_;}
	CPaintManagerUI* pPM = pMgr->GetManager();
	if (!pPM) {goto _Exit_;}

	if (NULL != ::SetTimer(pPM->GetPaintWindow(), (UINT_PTR)pTimerParam, uElapse, lpTimerFunc)) {
		nRet = (SQUserPointer)pTimerParam;
	} else {
		nRet = NULL;
	}

_Exit_:
	sq_pushuserpointer(v, nRet);
	return 1;
}

// SQInteger VwSetTimer(int uElapse, LPCTSTR pFunc, PVOID pParam)
SQInteger VwSetTimer(HSQUIRRELVM v)
{
	return _VwSetTimerInternal(v, _VwSetTimerFunc);
}

// SQInteger VwSetTimeout(int uElapse, LPCTSTR pFunc, PVOID pParam)
SQInteger VwSetTimeout(HSQUIRRELVM v)
{
	return _VwSetTimerInternal(v, _VwSetTimeoutFunc);
}

//   BOOL VwKillTimer(SQInteger uIDEvent)
SQInteger VwKillTimer(HSQUIRRELVM v)
{
    SQUserPointer     uIDEvent   = 0;
    SQInteger         nargs      = sq_gettop(v);

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_USERPOINTER  != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getuserpointer(v, 2, &uIDEvent);

    CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
    if (!pMgr) {goto _Exit_;}
    CPaintManagerUI* pPM = pMgr->GetManager();
    if (!pPM) {goto _Exit_;}

    ::KillTimer(pPM->GetPaintWindow(), (UINT_PTR)uIDEvent);
    delete (SetTimerParam*)uIDEvent;

_Exit_:
	sq_pushbool(v, TRUE);
    return 1;
}

//   void VwUpdateLayout(HWND hWnd)
SQInteger VwUpdateLayout(HSQUIRRELVM v)
{
	SQInteger         nargs            = sq_gettop(v);
	SQInteger         nWnd             = 0;
	HWND              hWnd             = NULL;
	CPaintManagerUI*  pPM              = NULL;

	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER  != sq_gettype(v, 2)) {goto _Exit_;}

	sq_getinteger(v, 2, &nWnd);

	if (-1 == nWnd) {
		CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
		if (!pMgr) {goto _Exit_;}
		pPM = pMgr->GetManager();
		if (!pPM) {goto _Exit_;}
	} else {
		pPM = QiHwHandleToWin(nWnd)->pWinObj->GetPM();
		if (!pPM) {goto _Exit_;}
	}

	pPM->UpdateLayout();

_Exit_:
	return 1;
}

// void VwSetWindowPos(HWND hWnd, HWND hWndlnsertAfter,int X, int Y, int cx, int cy, UNIT Flags)
SQInteger VwSetWindowPos(HSQUIRRELVM v)
{
    SQInteger         uIDEvent         = 0;
    SQInteger         nargs            = sq_gettop(v);
    SQInteger         nWnd             = 0;
    SQInteger         nWndlnsertAfter  = NULL;
    HWND              hWnd             = NULL;
    HWND              hWndlnsertAfter  = NULL;
    int               X                = 0;
    int               Y                = 0;
    int               cx               = 0;
    int               cy               = 0;
    UINT              uFlags           = 0;

    if (!v || 7 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 3)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 4) && OT_FLOAT  != sq_gettype(v, 4)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 5) && OT_FLOAT  != sq_gettype(v, 5)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 6) && OT_FLOAT  != sq_gettype(v, 6)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 7) && OT_FLOAT  != sq_gettype(v, 7)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 8)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);
    sq_getinteger(v, 3, &nWndlnsertAfter);
    sq_getinteger(v, 4, &X);
    sq_getinteger(v, 5, &Y);
    sq_getinteger(v, 6, &cx);
    sq_getinteger(v, 7, &cy);
    sq_getinteger(v, 8, (SQInteger*)&uFlags);

    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    if (HWND_TOP == (HWND)nWndlnsertAfter    ||
        HWND_BOTTOM == (HWND)nWndlnsertAfter ||
        HWND_TOPMOST == (HWND)nWndlnsertAfter ||
        HWND_TOPMOST == (HWND)nWndlnsertAfter) {
        hWndlnsertAfter = (HWND)nWndlnsertAfter;
    } else {
        hWndlnsertAfter = QiHwHandleToWin(nWndlnsertAfter)->pWinObj->GetHWND();
    }

    ::SetWindowPos(hWnd, hWndlnsertAfter, X, Y, cx, cy, uFlags);
	//::MoveWindow( hWnd, X, Y, cx, cy, TRUE);

_Exit_:
    return 0;
}

//BOOL VwSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
SQInteger VwSetLayeredWindowAttributes(HSQUIRRELVM v)
{
    SQInteger         uIDEvent         = 0;
    SQInteger         nargs            = sq_gettop(v);
    SQInteger         nWnd             = 0;
    HWND              hWnd             = NULL;
    COLORREF          crKey            = 0;
    int               bAlpha           = 0;
    DWORD             dwFlags          = 0;

    if (!v || 4 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 3)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 4)) {goto _Exit_;}
    if (OT_INTEGER  != sq_gettype(v, 5)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);
    sq_getinteger(v, 3, (SQInteger*)&crKey);
    sq_getinteger(v, 4, (SQInteger*)&bAlpha);
    sq_getinteger(v, 5, (SQInteger*)&dwFlags);

    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    //::SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(hWnd, crKey, (BYTE)bAlpha, dwFlags);
    ::RedrawWindow(hWnd, NULL, NULL,
        RDW_ERASE | RDW_INVALIDATE | 
        RDW_FRAME | RDW_ALLCHILDREN);

_Exit_:
    return 1;
}

//   int  VwGetPosX(VApiHandle hWnd)
SQInteger VwGetPosX(HSQUIRRELVM v)
{
    SQInteger      nargs         = sq_gettop(v);
    SQInteger      nWnd          = 0;
    WinMgrItem*    pWin          = NULL;
    HWND           hWnd          = NULL;
    SQInteger      nRet          = 0;
    RECT           rc            = {0};

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);
    
    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    ::GetWindowRect(hWnd, &rc);
    nRet = rc.left;

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

//   int  VwGetPosY(VApiHandle hWnd)
SQInteger VwGetPosY(HSQUIRRELVM v)
{
    SQInteger      nargs         = sq_gettop(v);
    SQInteger      nWnd          = 0;
    WinMgrItem*    pWin          = NULL;
    HWND           hWnd          = NULL;
    SQInteger      nRet          = 0;
    RECT           rc            = {0};

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);

    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    ::GetWindowRect(hWnd, &rc);
    nRet = rc.top;

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

//   int  VwGetPosWight(VApiHandle hWnd)
SQInteger VwGetPosWight(HSQUIRRELVM v)
{
    SQInteger      nargs         = sq_gettop(v);
    SQInteger      nWnd          = 0;
    WinMgrItem*    pWin          = NULL;
    HWND           hWnd          = NULL;
    SQInteger      nRet          = 0;
    RECT           rc            = {0};

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);

    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    ::GetWindowRect(hWnd, &rc);
    nRet = rc.right - rc.left;

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

//   int  VwGetPosHight(VApiHandle hWnd)
SQInteger VwGetPosHight(HSQUIRRELVM v)
{
    SQInteger      nargs         = sq_gettop(v);
    SQInteger      nWnd          = 0;
    WinMgrItem*    pWin          = NULL;
    HWND           hWnd          = NULL;
    SQInteger      nRet          = 0;
    RECT           rc            = {0};

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);

    if (-1 == nWnd) {
        CScriptMgr* pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        CPaintManagerUI* pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

    ::GetWindowRect(hWnd, &rc);
    nRet = rc.bottom - rc.top;

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

// HANDLE VwSnapShot(VApiHandle hWnd)
SQInteger VwSnapShot(HSQUIRRELVM v)
{
    SQInteger        nargs         = sq_gettop(v);
    SQInteger        nWnd          = 0;
    HWND             hWnd          = NULL;
	CUIImage*        pImg          = NULL;
	CScriptMgr*      pMgr          = NULL;
	CPaintManagerUI* pPM           = NULL;
	BOOL             bIsAlphaWin   = NULL;
	VApiHandle       hRet          = NULL;
	CUIRect          rt;

    if (!v || 1 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWnd);

    if (-1 == nWnd) {
        pMgr = (CScriptMgr*)sq_getforeignptr(v);
        if (!pMgr) {goto _Exit_;}
        pPM = pMgr->GetManager();
        if (!pPM) {goto _Exit_;}
        hWnd = pPM->GetPaintWindow();
    } else {
        hWnd = QiHwHandleToWin(nWnd)->pWinObj->GetHWND();
    }

	::GetWindowRect(hWnd, &rt);
	bIsAlphaWin = pPM->IsAlphaWin();
	pImg = new CUIImage();
	pImg->Create(pPM->GetPaintDC(), rt.GetWidth(), rt.GetHeight(), 0, TRUE);
	
	::PrintWindow(hWnd, pImg->GetDC(), 0);
	::SendMessage(hWnd, WM_PRINT, (WPARAM)pImg->GetDC(), PRF_NONCLIENT | PRF_CLIENT | PRF_ERASEBKGND | PRF_CHILDREN);

	hRet = QiHwObjToHandle(pImg);

_Exit_:
	sq_pushinteger(v, hRet);
	return 1;
}

// HANDLE VwCreateWin(
// VApiHandle hWndParent, int X, int Y, int nWidth, int nHeight, 
// WCHAR* wszRegName, WCHAR* wszNewName, BOOL bIsAlpha)
SQInteger VwCreateWin(HSQUIRRELVM v)
{
    SQInteger        nargs           = sq_gettop(v);
    SQInteger        nWndParent      = 0;
    HWND             hWndParent      = NULL;
    CUIImage*        pImg            = NULL;
    CScriptMgr*      pMgr            = NULL;
    CPaintManagerUI* pPM             = NULL;
    int              X               = 0;
    int              Y               = 0;
    int              nWidth          = 0;
    int              nHeight         = 0;
    const SQChar*    pwszRegName     = NULL;
    const SQChar*    pwszNewName     = NULL;
    WCHAR            wszNewName[60]  = {0};
    SQBool           bIsAlphaWin     = NULL;
    WinMgrItem*      pWinMgrIt       = NULL;
    HWND             hWnd            = NULL;
    CWindowWnd*      pWinObj         = NULL;
    SQInteger        nRet            = 0;
    CMarkupNode*     pRootXm         = NULL;

    if (!v || 8 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 4)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 5)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 6)) {goto _Exit_;}
    if (OT_STRING != sq_gettype(v, 7)) {goto _Exit_;}
    if (OT_STRING != sq_gettype(v, 8)) {goto _Exit_;}
    if (OT_BOOL != sq_gettype(v, 9)) {goto _Exit_;}

    sq_getinteger(v, 2, &nWndParent);
    sq_getinteger(v, 3, &X);
    sq_getinteger(v, 4, &Y);
    sq_getinteger(v, 5, &nWidth);
    sq_getinteger(v, 6, &nHeight);
    sq_getstring(v, 7, &pwszRegName);
    //sq_getinteger(v, 8, &wszNewName);
    sq_getbool(v, 9, &bIsAlphaWin);

    swprintf_s(wszNewName, 60, L"%x", ::GetTickCount());

    if (nWndParent) {
        hWndParent = QiHwHandleToWin(nWndParent)->pWinObj->GetHWND();
    }

    pMgr = (CScriptMgr*)sq_getforeignptr(v);
    if (!pMgr) {goto _Exit_;}
    pPM = pMgr->GetManager();
    if (!pPM || !pPM->GetWinMgr()) {goto _Exit_;}

    pWinMgrIt = pPM->GetWinMgr()->FindWinByName(pwszRegName);
    if (!pWinMgrIt) {goto _Exit_;}

    // 新建立个模板，因为需要可重复用以前的名字
    pRootXm = new CMarkupNode(*pWinMgrIt->pWinXML);
    pWinObj = new CWindowTemplate(pWinMgrIt->pWinObj);
    pWinObj->GetPM()->SetWinMgr(pPM->GetWinMgr());
    pWinObj->SetDefaultResource(pRootXm);

    pWinMgrIt = pPM->GetWinMgr()->AddOneWin(pWinObj, 
        wszNewName, pRootXm, X, Y, nWidth, 
        nHeight, pWinMgrIt->dwStyle, pWinMgrIt->dwExStyle, &pWinMgrIt->ExInfo);

    if (!bIsAlphaWin) {
        hWnd = pWinObj->Create(hWndParent, pwszNewName,
            pWinMgrIt->dwStyle, pWinMgrIt->dwExStyle,
            X, Y, nWidth, nHeight, 0);
    } else {
        hWnd = pWinObj->CreateAlphaWin(hWndParent, pwszNewName,
            pWinMgrIt->dwStyle | WS_EX_LAYERED, pWinMgrIt->dwExStyle,
            X, Y, nWidth, nHeight);
    }

    if (!hWnd) {goto _Exit_;}

    nRet = QiHwObjToHandle(pWinMgrIt);

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

//////////////////////////////////////////////////////////////////////////
// ViIp 接口表示CFastImagePanelUI类的成员函数
// BOOL   ViIpSetImgByMem(VApiHandle hImgPaneCtrl, VApiHandle hImg)
SQInteger ViIpSetImgByMem(HSQUIRRELVM v)
{
	SQInteger          nargs         = sq_gettop(v);
	BOOL               bRet          = FALSE;
	SQInteger          hImg          = 0;
	CUIImage*          pImg          = NULL;
	CFastImagePanelUI* pImgPaneCtrl  = NULL;
	SQInteger          hImgPaneCtrl  = NULL;

	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &hImgPaneCtrl);
	sq_getinteger(v, 3, &hImg);

	pImg = (CUIImage*)QiHwHandleToObj(hImg);
	pImgPaneCtrl = (CFastImagePanelUI*)QiHwHandleToObj(hImgPaneCtrl);
	if (!pImg || !pImgPaneCtrl) {goto _Exit_;}
	
	bRet = pImgPaneCtrl->SetImgByMem(pImg);

_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Tween类

// HANDLE TwCreateTweenParam(float fTime, short nTransition = EXPO, short nEquation = EASE_OUT, float fDelay = 0)
SQInteger TwCreateTweenParam(HSQUIRRELVM v)
{
    SQInteger          nargs         = sq_gettop(v);
    SQInteger          nRet          = NULL;
    SQInteger          fTime         = 0;
    SQInteger          nTransition   = EXPO;
    SQInteger          nEquation     = EASE_OUT;
    SQInteger          fDelay        = 0;
    TweenerParam*      pParam        = NULL;

    if (!v || 4 + 1 != nargs) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 4)) {goto _Exit_;}
    if (OT_INTEGER != sq_gettype(v, 5)) {goto _Exit_;}

    sq_getinteger(v, 2, &fTime);
    sq_getinteger(v, 3, &nTransition);
    sq_getinteger(v, 4, &nEquation);
    sq_getinteger(v, 5, &fDelay);

    pParam = new TweenerParam(fTime, nTransition, nEquation, fDelay);
    if (!pParam) {goto _Exit_;}

    nRet = QiHwObjToHandle(pParam);

_Exit_:
    sq_pushinteger(v, nRet);
    return 1;
}

// BOOL TwAddProperty(HANDLE hTweenParam, float valor, float valorFinal)
SQInteger TwAddProperty(HSQUIRRELVM v)
{
	SQInteger          nargs         = sq_gettop(v);
	SQBool             bRet          = FALSE;
	TweenerParam*      pTweenParam   = NULL;
	SQInteger          nTweenParam   = NULL;
	SQInteger          fValor        = 0;
	SQInteger          fValorFinal   = 0;
	TweenerParam*      pParam        = NULL;

	if (!v || 3 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 4)) {goto _Exit_;}

	sq_getinteger(v, 2, &nTweenParam);
	sq_getinteger(v, 3, &fValor);
	sq_getinteger(v, 4, &fValorFinal);

	pTweenParam = (TweenerParam*)QiHwHandleToObj(nTweenParam);
	if (!pTweenParam) {goto _Exit_;}

	pTweenParam->addProperty(fValor, fValorFinal);
	bRet = TRUE;

_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

// HANDLE TwAddTween(HANDLE hTweenParam)
SQInteger TwAddTween(HSQUIRRELVM v)
{
	SQInteger          nargs         = sq_gettop(v);
	SQInteger          nRet          = 0;
	TweenerParam*      pTweenParam   = NULL;
	SQInteger          nTweenParam   = NULL;
	Tweener*           pTweener      = NULL;

	pTweener = new Tweener();
	if (!pTweener) {goto _Exit_;}

	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

	sq_getinteger(v, 2, &nTweenParam);

	pTweenParam = (TweenerParam*)QiHwHandleToObj(nTweenParam);
	if (!pTweenParam) {goto _Exit_;}

	pTweener->addTween(*pTweenParam);
	delete pTweenParam;

	nRet = QiHwObjToHandle(pTweener);

_Exit_:
	sq_pushinteger(v, nRet);
	return 1;
}

// BOOL TwStepTween(HANDLE hTween, long currentMillis)
SQInteger TwStepTween(HSQUIRRELVM v)
{
	SQInteger          nargs          = sq_gettop(v);
	SQBool             bRet           = FALSE;
	SQInteger          nTweener       = NULL;
	SQInteger          currentMillis  = 0;
	Tweener*           pTweener       = NULL;

	if (!v || 2 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 3)) {goto _Exit_;}

	sq_getinteger(v, 2, &nTweener);
	sq_getinteger(v, 3, &currentMillis);

	pTweener = (Tweener*)QiHwHandleToObj(nTweener);
	if (!pTweener) {goto _Exit_;}

	pTweener->step(currentMillis);
	bRet = TRUE;

_Exit_:
	sq_pushbool(v, bRet);
	return 1;
}

// arr TwQueryTween(HANDLE hTween)
SQInteger TwQueryTween(HSQUIRRELVM v)
{
	SQInteger          nargs         = sq_gettop(v);
	SQBool             bRet          = FALSE;
	SQInteger          nTween        = NULL;
	Tweener*           pTweener      = NULL;
	std::vector<TweenerProperty> arrProperties;
	std::vector<TweenerProperty>::iterator it;

	if (!v || 1 + 1 != nargs) {goto _Exit_;}
	if (OT_INTEGER != sq_gettype(v, 2)) {goto _Exit_;}

	sq_getinteger(v, 2, &nTween);

	pTweener = (Tweener*)QiHwHandleToObj(nTween);
	if (!pTweener) {goto _Exit_;}

	pTweener->QueryTween(0, arrProperties);
	if (arrProperties.empty()) {goto _Exit_;}

	sq_newarray(v, 0/*arrProperties.size()*/);

	for (it = arrProperties.begin(); it != arrProperties.end(); ++it ) {
		sq_pushinteger(v, it->curValue);
		sq_arrayappend(v,-2);
	}

	bRet = TRUE;

_Exit_:
	if (!bRet) {
		sq_pushnull(v);
	}

	return 1;
}

// float TwGetElapsedTimeMillis()
SQInteger TwGetElapsedTimeMillis(HSQUIRRELVM v)
{
	LARGE_INTEGER nFreq     = {0};
	LARGE_INTEGER nLastTime = {0};

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLastTime);

	SQInteger nMillis = (SQInteger)(nLastTime.QuadPart/nFreq.QuadPart);
	sq_pushinteger(v, nMillis);
	return 1;
}
