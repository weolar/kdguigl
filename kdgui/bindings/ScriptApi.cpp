#include <UIlib.h>
#include "Core/UIManager.h"
#include "ScriptApi.h"
#include "script/scripthelp/SquirrelObject.h"
#include "Core/ScheduledAction.h"
#include "Core/DOMTimer.h"

SQInteger SqDOMWindowSetTimeoutOrInterval(HSQUIRRELVM v, bool isTimeout) {
	CPageManager* pageMgr = static_cast<CPageManager*>(sq_getforeignptr(v));

	HSQOBJECT callBack;
	sq_resetobject(&callBack); 
	KDASSERT (SQ_SUCCEEDED(sq_getstackobj(v, 2, &callBack)) && OT_CLOSURE == callBack._type);

	//sq_addref(v, &callBack); // 注意，这里会导致这个闭包永远也释放不了

	int delay = 0;
	KDASSERT (SQ_SUCCEEDED(sq_getinteger(v, 3, &delay)));

	DOMTimer* pDOMTimer = pageMgr->GetDOMTimer();
	ScheduledAction* action = new ScheduledAction(v, &callBack, delay, isTimeout, pageMgr->GetThreadTimers(), pDOMTimer);

	int result = pDOMTimer->Install(action);
	action->SetTimerId(result);

	sq_poptop(v);

	sq_pushinteger(v, result);

	return 1;
}

SQInteger SqSetTimeout(HSQUIRRELVM v) {
	return SqDOMWindowSetTimeoutOrInterval(v, true);
}

SQInteger SqSetInterval(HSQUIRRELVM v) {
	return SqDOMWindowSetTimeoutOrInterval(v, false);
}

SQInteger SqClearTimeout(HSQUIRRELVM v) {
	CPageManager* pageMgr = static_cast<CPageManager*>(sq_getforeignptr(v));

	int timerId = 0;
	KDASSERT (SQ_SUCCEEDED(sq_getinteger(v, 2, &timerId)));
	pageMgr->GetDOMTimer()->RemoveById(timerId);

	return 0;
}

SQInteger SqClearInterval(HSQUIRRELVM v) {
	return SqClearTimeout(v);
}

#ifndef _MSC_VER

#include <time.h>

inline unsigned long GetTickCount()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#endif

SQInteger SqGetTickCount(HSQUIRRELVM v) {
	int count = (int)GetTickCount();
	if (count < 0)
		count += 0x7FFFFFFF;

	sq_pushinteger(v, count);
	return 1;
}

SQInteger SqGetScreenCursorPos(HSQUIRRELVM v) {
	POINT ptCursor = {0};
	//::GetCursorPos(&ptCursor);
	DebugBreak();

	sq_newtable(v);

	sq_pushstring(v, _SC("x"), -1);
	sq_pushinteger(v, ptCursor.x);
	sq_newslot(v, -3, SQFalse );

	sq_pushstring(v, _SC("y"), -1);
	sq_pushinteger(v, ptCursor.y);
	sq_newslot(v, -3, SQFalse );

	return 1;
}

SQInteger SqGetClientCursorPos(HSQUIRRELVM v) {
	CPageManager* pageMgr = static_cast<CPageManager*>(sq_getforeignptr(v));
	HWND hWnd = pageMgr->GetHWND();
	int top = sq_gettop(v);

	POINT ptCursor = {0};
	//::GetCursorPos(&ptCursor);
	DebugBreak();
	//::ScreenToClient(hWnd, &ptCursor);

	sq_newtable(v);

	sq_pushstring(v, _SC("x"), -1);
	sq_pushinteger(v, ptCursor.x);
	sq_newslot(v, -3, SQFalse );

	sq_pushstring(v, _SC("y"), -1);
	sq_pushinteger(v, ptCursor.y);
	sq_newslot(v, -3, SQFalse );

	return 1;
}

SQInteger SqGetAsyncKeyState(HSQUIRRELVM v) {
	SQInteger nVirtKey = 0;
	sq_getinteger(v, 2, &nVirtKey);
	DebugBreak();
	//sq_pushinteger(v, GetAsyncKeyState(nVirtKey));

	return 1;
}

SQInteger SqGetCursorPosX(HSQUIRRELVM v) {
	POINT point = {0};
	//GetCursorPos(&point);
	DebugBreak();
	sq_pushinteger(v, point.x);
	return 1;
}

SQInteger SqGetCursorPosY(HSQUIRRELVM v) {
	POINT point = {0};
	//GetCursorPos(&point);
	DebugBreak();
	sq_pushinteger(v, point.y);
	return 1;
}

SQUIRREL_API void* SbuGetPagePtr(HSQUIRRELVM v)
{
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	return pManager->GetPagePtr();
}

SQUIRREL_API void* SbuGetKdPageHandle(HSQUIRRELVM v)
{
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	return pManager->GetWrap();
}

SQUIRREL_API HWND SbuGetHWND(HSQUIRRELVM v)
{
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	return pManager->GetHWND();
}

// 无标题栏对话框的拖动
SQInteger SqIsDraggableRegionNcHitTest(HSQUIRRELVM v)
{
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	pManager->SetIsDraggableRegionNcHitTest();
	return 0;
}