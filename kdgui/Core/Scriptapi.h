#if !defined(AFX_UICONTROLS_H__A3117604_DFA8_499f_BCD5_6F3FE30A8445__INCLUDED_)
#define AFX_UICONTROLS_H__A3117604_DFA8_499f_BCD5_6F3FE30A8445__INCLUDED_

#pragma once

#include "uidefene.h"
#include "UIManager.h"
#include "script/include/squirrel.h"
#include "script/include/sqstdaux.h"
#include "UIScript.h"

#define QiScriptMsgNameMaxLen 40
#define QiScriptMsgNameMaxSize (QiScriptMsgNameMaxLen + 1) // °üº¬0

typedef int VApiHandle;

typedef struct _SetTimerParam
{
    HSQUIRRELVM   v;
    HSQOBJECT     Obj;
    CStdString    sFunc;
	~_SetTimerParam() {};
}SetTimerParam;

SQInteger ViRegSysMsgCallBack(HSQUIRRELVM v);
SQInteger ViRegCtrlMsgCallBack(HSQUIRRELVM v);

SQInteger ViHwFindControl(HSQUIRRELVM v);

UINode* QiHwHandleToCtrl(int Handle);
WinMgrItem* QiHwHandleToWin(int Handle);
VApiHandle QiHwCtrlToHandle(UINode* pCtrl);
VApiHandle QiHwWinToHandle(WinMgrItem* pCtrl);
VApiHandle QiHwObjToHandle(PVOID pObj);
PVOID QiHwHandleToObj(VApiHandle hObj);

SQInteger ViMessageBox(HSQUIRRELVM v);

SQInteger ViCbCreateCtrl(HSQUIRRELVM v);
SQInteger ViCbAddCtrl(HSQUIRRELVM v);
SQInteger ViCbGetName(HSQUIRRELVM v);
SQInteger ViCbSetName(HSQUIRRELVM v);
SQInteger ViCbGetParent(HSQUIRRELVM v);
SQInteger ViCbGetText(HSQUIRRELVM v);
SQInteger ViCbSetText(HSQUIRRELVM v);
SQInteger ViCbGetToolTip(HSQUIRRELVM v);
SQInteger ViCbSetToolTip(HSQUIRRELVM v);
SQInteger ViCbSetFocus(HSQUIRRELVM v);
SQInteger ViCbIsEnabled(HSQUIRRELVM v);
SQInteger ViCbIsFocused(HSQUIRRELVM v);
SQInteger ViCbGetPosX(HSQUIRRELVM v);
SQInteger ViCbGetPosY(HSQUIRRELVM v);
SQInteger ViCbGetPosWight(HSQUIRRELVM v);
SQInteger ViCbGetPosHight(HSQUIRRELVM v);
SQInteger ViCbSetPos(HSQUIRRELVM v);
SQInteger ViCbSetiPos(HSQUIRRELVM v);
SQInteger ViCbSetZrol(HSQUIRRELVM v);
SQInteger ViCbSetAlpha(HSQUIRRELVM v);
SQInteger ViCbApplyAttributeList(HSQUIRRELVM v);

SQInteger VwSetTimer(HSQUIRRELVM v);
SQInteger VwSetTimeout(HSQUIRRELVM v);
SQInteger VwKillTimer(HSQUIRRELVM v);
SQInteger VwUpdateLayout(HSQUIRRELVM v);
SQInteger VwSetWindowPos(HSQUIRRELVM v);
SQInteger VwSetLayeredWindowAttributes(HSQUIRRELVM v);
SQInteger VwGetPosX(HSQUIRRELVM v);
SQInteger VwGetPosY(HSQUIRRELVM v);
SQInteger VwGetPosWight(HSQUIRRELVM v);
SQInteger VwGetPosHight(HSQUIRRELVM v);
SQInteger VwSnapShot(HSQUIRRELVM v);
SQInteger VwCreateWin(HSQUIRRELVM v);

SQInteger ViIpSetImgByMem(HSQUIRRELVM v);

SQInteger TwCreateTweenParam(HSQUIRRELVM v);
SQInteger TwAddProperty(HSQUIRRELVM v);
SQInteger TwStepTween(HSQUIRRELVM v);
SQInteger TwAddTween(HSQUIRRELVM v);
SQInteger TwQueryTween(HSQUIRRELVM v);
SQInteger TwGetElapsedTimeMillis(HSQUIRRELVM v);

#endif