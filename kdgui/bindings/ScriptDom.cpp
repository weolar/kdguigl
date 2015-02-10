
#include <UIlib.h>
#include <wtf/UtilHelp.h>

#include "ScriptDom.h"
#include "KqPropsPushHelp.h"
#include "script/scripthelp/SquirrelObject.h"

#include "Dom/VectorNodeImpl.h"

#include "Dom/ImageNode.h"
#include "Dom/RootNode.h"
#include "Dom/LinearGradientNode.h"
#include "Dom/RadialGradientNode.h"
#include "Dom/StopNode.h"
#include "Dom/TextNode.h"
#include "Dom/GNode.h"
#include "Dom/ClipPathNode.h"
#include "Dom/DefsNode.h"
//#include "Dom/MaskNode.h"
//#include "Dom/RichEditBaseNode.h"

#include "Dom/NodeAnim.h"
#include "bindings/SqEventListener.h"
#include "script/include/sqstdaux.h"

static UINode* CreateNodeByClass(CPageManager* pManager, LPCTSTR pstrClass) {
	size_t cchLen = _tcslen(pstrClass);
	UINode* node = NULL;
	if( _tcsicmp(pstrClass, _SC("Rect")) == 0 )                    node = new RectNode;
	else if( _tcsicmp(pstrClass, _SC("Circle")) == 0 )             node = new CircleNode;
	else if( _tcsicmp(pstrClass, _SC("Ellipse")) == 0 )            node = new EllipseNode;
	else if( _tcsicmp(pstrClass, _SC("Line")) == 0 )               node = new LineNode;
	else if( _tcsicmp(pstrClass, _SC("Path")) == 0 )               node = new PathNode;
	else if( _tcsicmp(pstrClass, _SC("Image")) == 0 )              node = new ImageNode;
	else if( _tcsicmp(pstrClass, _SC("Root")) == 0 )               node = new RootNode;
	else if( _tcsicmp(pstrClass, _SC("LinearGradient")) == 0 )     node = new LinearGradientNode;
	else if( _tcsicmp(pstrClass, _SC("RadialGradient")) == 0 )     node = new RadialGradientNode;
	else if( _tcsicmp(pstrClass, _SC("Stop")) == 0 )               node = new StopNode;
	else if( _tcsicmp(pstrClass, _SC("Text")) == 0 )               node = new TextNode;
	else if( _tcsicmp(pstrClass, _SC("G")) == 0 )                  node = new GNode;
	else if( _tcsicmp(pstrClass, _SC("ClipPath")) == 0 )           node = new ClipPathNode;
	else if( _tcsicmp(pstrClass, _SC("Defs")) == 0 )               node = new DefsNode;
	//else if( _tcsicmp(pstrClass, _SC("Mask")) == 0 )               node = new MaskNode;
	//else if( _tcsicmp(pstrClass, _SC("RichEditBase")) == 0 )       node = new RichEditBaseNode;
	else {
		CStdString out = _SC("找不到此类型的节点:");
		out += pstrClass;
		out += _SC("\n");
		OutputDebugString(out);
		KDASSERT(FALSE);
	}
	
	if (node)
		node->SetManager(pManager);
	else
		KDASSERT(FALSE);
	return node;
}

static SQInteger _SqReleaseHook(SQUserPointer body, SQInteger size) {
	UINode* node = (UINode*)body;
	if (!node->GetManager()->IsScriptDestroying())
		node->deref();
	
	return 1;
}

static void PushSq(HSQUIRRELVM v, UINode* node) {
	if (!node) {
		sq_pushnull(v);
		return;
	}
	
	// 节点在获取的时候确保已经ref，此处不需要再node->ref();
	SbuCreateNativeClassInstance(v, _SC("KSqNode"), node, _SqReleaseHook);
}

SQInteger Sq$(HSQUIRRELVM v) {
	StackHandler sa(v);
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));

	UINode* node = pManager->GetNodeByID(sa.GetString(2));
	PushSq(v, node);
	return 1;
}

SQInteger Sq$$(HSQUIRRELVM v) {
	StackHandler sa(v);
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));

	UINode* node = CreateNodeByClass(pManager, sa.GetString(2));
	node->ref();
	PushSq(v, node);

	if (0)
		sqstd_printcallstack(v);

	return 1;
}

SQInteger SqSvgRoot(HSQUIRRELVM v) {
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	PushSq(v, pManager->GetRoot());
	return 1;
}

SQInteger SqSvgDoc(HSQUIRRELVM v) {
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	PushSq(v, pManager->GetDocNode());
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// WEBKIT_EXPORT KQuery* val();
_MEMBER_FUNCTION_IMPL(KSqNode, val)
{
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);

	CStdString valueString = self->GetAttr(sa.GetString(2));
	return sa.Return(valueString.GetString());
}

// 使用table或者数组作为输入
_MEMBER_FUNCTION_IMPL(KSqNode, attr) {
	_CHECK_SELF(UINode, KSqNode);

	KqPropsPushHelp* props = KQPUSH();
	KqGetParamFromSq(v, 2, props);

	WTF::Vector<AnimPropPair> copyProps;

	if (0 != props->m_props.size()%2) {
		DebugBreak();
		goto Exit0;
	}

	props->reserve(props->m_props.size());

	self->SetAttrBegin();
	for (size_t i = 0; i < props->m_props.size(); i+= 2) {
		KqPropsTypeVal valType =  props->m_props[i + 1].type; // 奇数项是值，必须是浮点型或字符串

		if (KqPropsTypeValString != valType && KqPropsTypeValFloat != valType) // float
		{ KDASSERT(FALSE); continue; }

		KqPropsTypeVal nameType =  props->m_props[i].type; // 偶数项是名称，必须是字符串
		if (KqPropsTypeValString != nameType) 
		{ KDASSERT(FALSE); continue; }

		if (KqPropsTypeValFloat == valType) {
			SQChar val[nUHFastFloat2StrFormatBufLen] = {0};
			UHFastFloat2StrFormat(props->m_props[i + 1].floatVal, val);
			self->SetAttr(props->m_props[i].stringVal, val);
		} else if (KqPropsTypeValString == valType) {
			// 为了兼容老kdgui2的bug
			if (11 == props->m_props[i].stringVal.GetLength() && -1 != props->m_props[i].stringVal.Find(_SC("translate-")))
				continue;
			
			self->SetAttr(props->m_props[i].stringVal.GetString(),
				props->m_props[i + 1].stringVal.GetString());
		}
	}
	self->SetAttrEnd();

Exit0:
	props->deref();

	if (0)
		sqstd_printcallstack(v);

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, childrenSize) {
	_CHECK_SELF(UINode, KSqNode);
	sq_pushinteger(v, self->GetChilds()->size());
	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, appendTo) {
	_CHECK_SELF(UINode, KSqNode);
	_CHECK_INST_PARAM(parent, 2, UINode, KSqNode);
	parent->AppendChild(self);

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, focus) {
	_CHECK_SELF(UINode, KSqNode);
	self->SetFocus();
	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, blur) {
	_CHECK_SELF(UINode, KSqNode);
	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, bind) {
	_CHECK_SELF(UINode, KSqNode);

	const SQChar *eventName;

	KDASSERT(SQ_SUCCEEDED(sq_getstring(v, 2, &eventName)));

	HSQOBJECT callBack;
	sq_resetobject(&callBack); 
	KDASSERT (SQ_SUCCEEDED(sq_getstackobj(v, 3, &callBack)) && OT_CLOSURE == callBack._type);

	sq_addref(v, &callBack); // now that you have a reference to the function you can store it somware in you app for late use(remeber to call sq_release when you are don with it

	SQBool useCapture;
	KDASSERT (SQ_SUCCEEDED(sq_getbool(v, 4, &useCapture)));

	HSQOBJECT callBackData;
	sq_resetobject(&callBackData); 
	KDASSERT(SQ_SUCCEEDED(sq_getstackobj(v, 5, &callBackData)));

	sq_addref(v, &callBackData);

	self->AddEventListener(UHEventNameToEnum(eventName), SqEventListener::Create(v, callBack, callBackData, !!useCapture));

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, unbind) {
	_CHECK_SELF(UINode, KSqNode);

	const SQChar *eventName;
	KDASSERT (SQ_SUCCEEDED(sq_getstring(v, 2, &eventName)));

	HSQOBJECT callBack;
	sq_resetobject(&callBack); 
	KDASSERT (SQ_SUCCEEDED(sq_getstackobj(v, 3, &callBack)) || OT_CLOSURE != callBack._type);

	sq_addref(v, &callBack);

	SQBool useCapture;
	KDASSERT (SQ_SUCCEEDED(sq_getbool(v, 4, &useCapture)));

	HSQOBJECT callBackData;
	sq_resetobject(&callBackData);
	sq_addref(v, &callBackData);

	PassRefPtr<EventListener> listener = SqEventListener::Create(v, callBack, callBackData, !!useCapture);
	self->RemoveEventListener(UHEventNameToEnum(eventName), listener.get());

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, clearBind) {
	_CHECK_SELF(UINode, KSqNode);
	self->ClearEventListener();

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, destroyMe) {
	_CHECK_SELF(UINode, KSqNode);

	self->DestroyTree();

	return 0;
}

_MEMBER_FUNCTION_IMPL(KSqNode, getNodePtr) {
	_CHECK_SELF(UINode, KSqNode);

	CStdString out;
	out.Format(_SC("%x"), self);
	sq_pushstring(v, out.GetString(), out.GetLength());
	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, getNode) {
	_CHECK_SELF(UINode, KSqNode);

	sq_pushinteger(v, (SQInteger)self);
	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, text) {
	_CHECK_SELF(UINode, KSqNode);

	const SQChar* text;
	KDASSERT (SQ_SUCCEEDED(sq_getstring(v, 2, &text)));

	self->Dispatch(_SC("SetTextOfTextNode"), 0, text, NULL, NULL);

	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, getText) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	CStdString* text = NULL;
	if (0 != self->Dispatch(_SC("GetTextOfTextNode"), 0, NULL, NULL, &text) || !text) {
		sq_pushnull(v);
		return 1;
	}

	sq_pushstring(v, text->GetString(), text->GetLength());
	return 1;
}

static SQInteger _animReleaseHook(SQUserPointer body, SQInteger size) {
	NodeAnim* anim = (NodeAnim*)body;
	if (!anim->GetManager()->IsScriptDestroying())
		anim->deref();
	return 0;
}

_MEMBER_FUNCTION_IMPL(KSqNode, anim) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);

	BOOL stopALL = sa.GetBool(2);
	NodeAnim* anim = self->Anim(!!stopALL);
	anim->ref();
	SbuCreateNativeClassInstance(v, _SC("KqAnim"), anim, _animReleaseHook);
	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, stopAllAnim) {
	_CHECK_SELF(UINode, KSqNode);

	self->ForceStopAllAnimAndDestroy();
	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KSqNode, getRef) {
	_CHECK_SELF(UINode, KSqNode);

	sq_pushinteger(v, self->GetRef());
	return 1;
}

// WEBKIT_EXPORT UINode* parent();
_MEMBER_FUNCTION_IMPL(KSqNode, parent) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	UINode* parent = self->GetParent();
	PushSq(v, parent);
	return 1;
	//return sa.Return(new_KQ(v, (*parent)));
}

// WEBKIT_EXPORT UINode* prev();
_MEMBER_FUNCTION_IMPL(KSqNode, prev) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	UINode* previousSibling = self->Prev();
	PushSq(v, previousSibling);

	return 1;
}

// WEBKIT_EXPORT UINode* sibling();
_MEMBER_FUNCTION_IMPL(KSqNode, sibling) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	UINode* nextSibling = self->Sibling();

	PushSq(v, nextSibling);
	return 1;
	//return sa.Return(new_KQ(v, (*nextSibling)));
}

// WEBKIT_EXPORT UINode* firstChild();
_MEMBER_FUNCTION_IMPL(KSqNode, firstChild) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	UINode* firstChild = self->FirstChild();

	PushSq(v, firstChild);
	return 1;
	//return sa.Return(new_KQ(v, (*firstChild)));
}

// WEBKIT_EXPORT UINode* lastChild();
_MEMBER_FUNCTION_IMPL(KSqNode, lastChild) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	UINode* lastChild = self->LastChild();

	PushSq(v, lastChild);
	return 1;
	//return sa.Return(new_KQ(v, (*lastChild)));
}

_MEMBER_FUNCTION_IMPL(KSqNode, find) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	const SQChar* findVal = sa.GetString(2);
	if (!findVal) {
		sq_pushnull(v);
		return 1;
	}

	UINode* findNode = self->FindByClass(findVal);
	if (!findNode) {
		sq_pushnull(v);
		return 1;
	}

	PushSq(v, findNode);
	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, realPos) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);
	IntRect rc = self->BoundingRectAbsolute();

	sq_newtable(v);

	sq_pushstring(v, _SC("x"), -1);
	sq_pushinteger(v, rc.x());
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, _SC("y"), -1);
	sq_pushinteger(v, rc.y());
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, _SC("width"), -1);
	sq_pushinteger(v, rc.width());
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, _SC("height"), -1);
	sq_pushinteger(v, rc.height());
	sq_newslot(v, -3, SQFalse);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, realTextSize) {
	_CHECK_SELF(UINode, KSqNode);
	
	IntRect rc = self->BoundingRectAbsolute();

	sq_newtable(v); // 新建一个table，用来返回给脚本，做返回值

	sq_pushstring(v, _SC("width"), -1);
	sq_pushinteger(v, rc.width());
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, _SC("height"), -1);
	sq_pushinteger(v, rc.height());
	sq_newslot(v, -3, SQFalse);

	return 1;
}

class KqAdditionalData : public KqNAdditionalData {
public:
	KqAdditionalData() {
		v = NULL;
		sq_resetobject(&o);
	}

	virtual void Uninit(bool bIsScriptDestroying) {
		if (v && !bIsScriptDestroying)
			sq_release(v, &o);
	}

	HSQOBJECT o;
	HSQUIRRELVM v;
};

SQInteger SetDataOrWidget (HSQUIRRELVM v, bool bDataOrWidget) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);

	KqAdditionalData* data = bDataOrWidget ? (KqAdditionalData*)self->Userdata() : (KqAdditionalData*)self->Widget();
	if (data) {
		ASSERT(ISREFCOUNTED((data->o)._type));
		SQUnsignedInteger refNum = sq_getrefcount(v, &data->o);
		ASSERT(0 < refNum);
		bDataOrWidget ? self->SetUserdata(NULL) : self->SetWidget(NULL); // 里面会delete data
	}

	HSQOBJECT o;
	if (!SQ_FAILED(sq_getstackobj(v, 2, &o)) && OT_NULL != o._type) {
		data = new KqAdditionalData;
		data->v = v;
		data->o = o;
		sq_addref(v, &data->o);
		bDataOrWidget ? self->SetUserdata(data) : self->SetWidget(data);
	}

	_RETURN_THIS(1);
}

SQInteger GetDataOrWidget (HSQUIRRELVM v, bool bDataOrWidget) {
	StackHandler sa(v);
	_CHECK_SELF(UINode, KSqNode);

	KqAdditionalData* data = bDataOrWidget ? (KqAdditionalData*)self->Userdata() : (KqAdditionalData*)self->Widget();
	if (!data) {
		sq_pushnull(v);
		return 1;
	}

	ASSERT(ISREFCOUNTED(data->o._type));
	//sq_addref(v, data);
	sq_pushobject(v, data->o);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KSqNode, setData) {
	return SetDataOrWidget(v, true);
}

_MEMBER_FUNCTION_IMPL(KSqNode, setWidget) {
	return SetDataOrWidget(v, false);
}

_MEMBER_FUNCTION_IMPL(KSqNode, data) {
	return GetDataOrWidget(v, true);
}

_MEMBER_FUNCTION_IMPL(KSqNode, widget) {
	return GetDataOrWidget(v, false);
}

_BEGIN_CLASS(KSqNode)
_MEMBER_FUNCTION(KSqNode, attr, 2, _SC("xa|t"))
_MEMBER_FUNCTION(KSqNode, val, 2, _SC("xs"))
_MEMBER_FUNCTION(KSqNode, appendTo, 2, _SC("xx"))
_MEMBER_FUNCTION(KSqNode, childrenSize, 1, _SC("x"))

// _MEMBER_FUNCTION(KSqNode, remove, 2, _SC("xx"))
_MEMBER_FUNCTION(KSqNode, destroyMe, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, focus, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, blur, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, bind, 5, _SC("xscb."))
_MEMBER_FUNCTION(KSqNode, unbind, 4, _SC("xscb"))
_MEMBER_FUNCTION(KSqNode, clearBind, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, realPos, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, text, 2, _SC("xs"))
_MEMBER_FUNCTION(KSqNode, getText, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, parent, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, prev, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, sibling, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, firstChild, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, lastChild, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, find, 2, _SC("xs"))

_MEMBER_FUNCTION(KSqNode, realTextSize,1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, setData, 2, _SC("x."))
_MEMBER_FUNCTION(KSqNode, data, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, setWidget, 2, _SC("x."))
_MEMBER_FUNCTION(KSqNode, widget, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, anim, 2, _SC("xb"))
_MEMBER_FUNCTION(KSqNode, stopAllAnim, 1, _SC("x"))

_MEMBER_FUNCTION(KSqNode, getNodePtr, 1, _SC("x"))
_MEMBER_FUNCTION(KSqNode, getNode, 1, _SC("x"))

_END_CLASS(KSqNode)

//////////////////////////////////////////////////////////////////////////

_DECL_CLASS(KqEvt);

_MEMBER_FUNCTION_IMPL(KqEvt, target) {
	_CHECK_SELF(SqPushEventStruct, KqEvt);
	UINode* target = self->event->pTarget;
	target->ref();
	PushSq(v, target);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, type) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	const SQChar* type = UHEnumToEventName(self->event->Type);
	if (!type)
		sq_pushnull(v);
	else
		sq_pushstring(v, type, -1);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, clientX) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, self->event->ptMouse.x);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, clientY) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, self->event->ptMouse.y);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, screenX) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, 0);
	DebugBreak();

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, screenY) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, 0);
	DebugBreak();

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, button) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, self->event->button);

	return 1;
}

_MEMBER_FUNCTION_IMPL(KqEvt, wheelDelta) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	sq_pushinteger(v, self->event->wheelDelta);

	return 1;
}

// _MEMBER_FUNCTION_IMPL(KqEvt, isMouseLeaveOrEnter)
// {
// 	StackHandler sa(v);
// 	_CHECK_SELF(SqPushEventStruct, KqEvt);
// 
// 	sq_pushbool(v, isMouseLeaveByEvt(self->event));
// 
// 	return 1;
// }
// true表示真正出去边界，而不是在子元素上
_MEMBER_FUNCTION_IMPL(KqEvt, isMouseLeave) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	if (!self->event->pTarget) {
		sq_pushbool(v, true);
		return 1;
	}

	int x = self->event->ptMouse.x;
	int y = self->event->ptMouse.y;

	// 这里使用Bounding判断不准确，因为矢量的笔触被我优化掉，虽然在Bounding内，但
	// 不在FindControl的结果内。所以会导致FindControlByPointButNoDispatchMsg
	// 发现已经离开m_pEventHover，但又在Bounding内。另外，如果矢量不规则，也会导致判断失误
#if 0
	IntRect rc = self->event->pTarget->BoundingRectAbsolute();
	SQBool b = !(x > rc.x() && x < rc.maxX() && y > rc.y() && y < rc.maxY());
#endif

	FloatPoint pt(self->event->ptMouse);
	SQBool b = TRUE;
	PassRefPtr<UINode> n = self->event->pTarget->FindControl(NULL, &pt, UIFIND_HITTEST, NULL);
	if (n.get())
		b = FALSE;
	sq_pushbool(v, b);

	return 1;
}
// 
// _MEMBER_FUNCTION_IMPL(KqEvt, keyCode)
// {
// 	StackHandler sa(v);
// 	_CHECK_SELF(SqPushEventStruct, KqEvt);
// 
// 	if (!self->event->isKeyboardEvent()) {
// 		sq_pushinteger(v, -1);
// 		return 1;
// 	}
// 
// 	KeyboardEvent* keyEvent = (KeyboardEvent*)self->event;
// 	sq_pushinteger(v, keyEvent->keyCode());
// 
// 	return 1;
// }
// 
// _MEMBER_FUNCTION_IMPL(KqEvt, charCode)
// {
// 	StackHandler sa(v);
// 	_CHECK_SELF(SqPushEventStruct, KqEvt);
// 
// 	if (!self->event->isKeyboardEvent()) {
// 		sq_pushinteger(v, -1);
// 		return 1;
// 	}
// 
// 	KeyboardEvent* keyEvent = (KeyboardEvent*)self->event;
// 	sq_pushinteger(v, keyEvent->charCode());
// 
// 	return 1;
// }
// 
// _MEMBER_FUNCTION_IMPL(KqEvt, preventDefault)
// {
// 	StackHandler sa(v);
// 	_CHECK_SELF(SqPushEventStruct, KqEvt);
// 
// 	self->event->preventDefault();
// 
// 	return 0;
// }

_MEMBER_FUNCTION_IMPL(KqEvt, timeStamp) {
	StackHandler sa(v);
	_CHECK_SELF(SqPushEventStruct, KqEvt);

	DWORD nTimeStamp = (DWORD)self->event->dwTimestamp;
	sq_pushinteger(v, nTimeStamp);

	return 1;
}

_BEGIN_CLASS(KqEvt)
_MEMBER_FUNCTION(KqEvt,target,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,type,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,clientX,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,clientY,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,screenX,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,screenY,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,button,1,_SC("x"))
// _MEMBER_FUNCTION(KqEvt,isMouseLeaveOrEnter,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,isMouseLeave,1,_SC("x"))
// _MEMBER_FUNCTION(KqEvt,keyCode,1,_SC("x"))
// _MEMBER_FUNCTION(KqEvt,charCode,1,_SC("x"))
// _MEMBER_FUNCTION(KqEvt,preventDefault,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,timeStamp,1,_SC("x"))
_MEMBER_FUNCTION(KqEvt,wheelDelta,1,_SC("x"))
_END_CLASS(KqEvt)

//////////////////////////////////////////////////////////////////////////

struct KaAnimStepOrEndCallbackInfo {
	HSQUIRRELVM v;
	HSQOBJECT callback;
	HSQOBJECT callbackData;
};

#ifndef NDEBUG
static RefCountedLeakCounter customWithCallbackCounter(_SC("customWithCallback"));
#endif

static bool WINAPI KaAnimStepCallback (float result, const SQChar* attr, int attrLen, void* param1, void* param2) {
	KaAnimStepOrEndCallbackInfo* info = (KaAnimStepOrEndCallbackInfo*)param1;
	if (!attr) {
#ifndef NDEBUG
		customWithCallbackCounter.decrement();
#endif
		sq_release(info->v, &info->callback);
		sq_release(info->v, &info->callbackData);
		WTF::fastFree(info);
		return true;
	}

	sq_addref(info->v, &info->callback); // 之所以要引用加一，是为了防止在脚本中把本对象删除掉

	SQInteger top = sq_gettop(info->v); // 保存原始堆栈大小
	sq_pushobject(info->v, info->callback);
	sq_pushroottable(info->v); // 松鼠的函数总是隐藏了一个this指针

	sq_pushfloat(info->v, (SQFloat)result);
	sq_pushstring(info->v, attr, attrLen);
	//sq_pushinteger(info->v, (SQInteger)attrLen);
	sq_pushobject(info->v, info->callbackData);

	if(SQ_FAILED(sq_call(info->v, 4, SQFalse, SQTrue))) 
		KDASSERT(FALSE);

	int listenerRefConut = sq_getrefcount(info->v, &info->callback);
	sq_release(info->v, &info->callback);

	if (1 == listenerRefConut)  // 如果为1，说明脚本里调用了removeEventListener
		info->callback._type = OT_NULL;

	sq_settop(info->v, top); // 还原堆栈

	return false;
}

static bool WINAPI KaAnimEndCallback (void* param1, void* param2) {
	KaAnimStepOrEndCallbackInfo* info = (KaAnimStepOrEndCallbackInfo*)param1;
	sq_addref(info->v, &info->callback); // 之所以要引用加一，是为了防止在脚本中把本对象删除掉

	SQInteger top = sq_gettop(info->v); // 保存原始堆栈大小
	sq_pushobject(info->v, info->callback);
	sq_pushroottable(info->v); // 松鼠的函数总是隐藏了一个this指针

	sq_pushobject(info->v, info->callbackData);

	if(SQ_FAILED(sq_call(info->v, 2, SQFalse, SQTrue))) 
		KDASSERT(FALSE);

	int listenerRefConut = sq_getrefcount(info->v, &info->callback);
	sq_release(info->v, &info->callback);
	if (1 == listenerRefConut)  // 如果为1，说明脚本里调用了removeEventListener
		info->callback._type = OT_NULL;

	sq_release(info->v, &info->callback); // 本函数执行完后 ，这个闭包也应该销毁了
	sq_release(info->v, &info->callbackData);

	sq_settop(info->v, top); // 还原堆栈
	WTF::fastFree(info);

	return true;
}

_DECL_CLASS(KqAnim);

_MEMBER_FUNCTION_IMPL(KqAnim, delay) {
	StackHandler sa(v);
	_CHECK_SELF(NodeAnim, KqAnim);
	int delayTime = sa.GetInt(2);
	self->Delay(delayTime);
	_RETURN_THIS(1);
}

_MEMBER_FUNCTION_IMPL(KqAnim, other) {
	_CHECK_SELF(NodeAnim, KqAnim);
	SbuCreateNativeClassInstance(v, _SC("KqAnim"), self->AttachedNode()->Anim(false)->ref(), _animReleaseHook);
	return 1;
}

_MEMBER_FUNCTION_IMPL(KqAnim, start) {
	_CHECK_SELF(NodeAnim, KqAnim);
	self->Start();
	_RETURN_THIS(1);
}

// KqPropsPushHelp* props, float dur,
//     KQueryEasing easing, KQueryTween tween
_MEMBER_FUNCTION_IMPL(KqAnim, custom) {
	StackHandler sa(v);
	_CHECK_SELF(NodeAnim, KqAnim);

	KqPropsPushHelp* pusher = KQPUSH();
	KqGetParamFromSq(v, 2, pusher);

	///BOOL stopALL = sa.GetBool(3);
	SQObjectType type = sq_gettype(v, 2);
	float dur = (OT_INTEGER == type ? (float)sa.GetFloat(3) : sa.GetInt(3));
	int easing = sa.GetInt(4);
	int tween = sa.GetInt(5);

	self->Custom(pusher, (int)dur, (KQueryEasing)easing, (KQueryTween)tween);

	_RETURN_THIS(1);
}

static SQInteger SQUIRREL_CALL KqSetAllAnimFinishCallback(HSQUIRRELVM v) {
	_CHECK_SELF(NodeAnim, KqAnim);

	HSQOBJECT callback;
	sq_resetobject(&callback); 
	if (SQ_FAILED(sq_getstackobj(v, 2, &callback)) || OT_CLOSURE != callback._type)
		KDASSERT(false);
	sq_addref(v, &callback);

	HSQOBJECT callbackData;
	sq_resetobject(&callbackData); 
	if (SQ_FAILED(sq_getstackobj(v, 3, &callbackData)))
		KDASSERT(false);
	sq_addref(v, &callbackData);

	KaAnimStepOrEndCallbackInfo* info = (KaAnimStepOrEndCallbackInfo*)WTF::fastMalloc(sizeof(KaAnimStepOrEndCallbackInfo));
	info->callback = callback;
	info->callbackData = callbackData;
	info->v = v;
	self->SetAllAnimFinishCallback(KaAnimEndCallback, (void*)info, 0);

	_RETURN_THIS(1);
}

// AnimEndCallback callback, void* callbackParam1
_MEMBER_FUNCTION_IMPL(KqAnim, setAllAnimFinishCallback) {
	return KqSetAllAnimFinishCallback(v);
}

_MEMBER_FUNCTION_IMPL(KqAnim, finish) {
	return KqSetAllAnimFinishCallback(v);
}

static SQInteger SQUIRREL_CALL KqAnimAnimate(HSQUIRRELVM v, bool bNewFunc) {
	//notImplemented(); // 本函数有bug！
	StackHandler sa(v);
	_CHECK_SELF(NodeAnim, KqAnim);

	KqPropsPushHelp* pusher = KQPUSH();
	KqGetParamFromSq(v, 2, pusher);

	//BOOL stopALL = sa.GetBool(3);
	SQObjectType type = sq_gettype(v, 3);
	float dur = (OT_INTEGER == type ? (float)sa.GetFloat(3) : sa.GetInt(3));
	int easing = sa.GetInt(4);
	int tween = sa.GetInt(5);
	int stepLen = sa.GetInt(6);

	type = sq_gettype(v, 7);
	float p0 = (OT_INTEGER == type ? (float)sa.GetFloat(7) : sa.GetInt(7));

	type = sq_gettype(v, 8);
	float p1 = (OT_INTEGER == type ? (float)sa.GetFloat(8) : sa.GetInt(8));

	HSQOBJECT stepCallback;
	sq_resetobject(&stepCallback); 
	if (SQ_FAILED(sq_getstackobj(v, 9, &stepCallback)) || OT_CLOSURE != stepCallback._type)
		KDASSERT(FALSE);
	sq_addref(v, &stepCallback);

	HSQOBJECT stepCallbackData;
	sq_resetobject(&stepCallbackData); 
	if (bNewFunc && SQ_FAILED(sq_getstackobj(v, 10, &stepCallbackData)))
		KDASSERT(FALSE);
	sq_addref(v, &stepCallbackData);

#ifndef NDEBUG
	customWithCallbackCounter.increment();
#endif
	KaAnimStepOrEndCallbackInfo* info = (KaAnimStepOrEndCallbackInfo*)WTF::fastMalloc(sizeof(KaAnimStepOrEndCallbackInfo));
	info->callback = stepCallback;
	info->callbackData = stepCallbackData;
	info->v = v;
	self->CustomWithCallback(pusher, (int)dur, (KQueryEasing)easing, (KQueryTween)tween, 
		stepLen, p0, p1, KaAnimStepCallback, info, 0);

	_RETURN_THIS(1);
}

// KqPropsPushHelp* props, float dur,
//     KQueryEasing easing, KQueryTween tween, int stepLen, float p0, float p1, AnimStepCallback stepCallback, void* param
// m_node.anim(true).customWithCallback([m_key, m_endValue], m_dur, m_easing, m_tween, m_stepValue, 0, 0, 
//   function(result, att, arg) {})
_MEMBER_FUNCTION_IMPL(KqAnim, customWithCallback) {
	return KqAnimAnimate(v, false);
}

// m_node.anim(true).customWithCallback([m_key, m_endValue], m_dur, m_easing, m_tween, m_stepValue, 0, 0, 
//   function(result, att, arg) {}, null);
_MEMBER_FUNCTION_IMPL(KqAnim, animate) {
	return KqAnimAnimate(v, true);
}

_BEGIN_CLASS(KqAnim)
_MEMBER_FUNCTION(KqAnim,delay,2,_SC("xi"))
_MEMBER_FUNCTION(KqAnim,other,1,_SC("x"))
_MEMBER_FUNCTION(KqAnim,start,1,_SC("x"))
_MEMBER_FUNCTION(KqAnim,setAllAnimFinishCallback,3,_SC("xc."))
_MEMBER_FUNCTION(KqAnim,finish,3,_SC("xc."))
_MEMBER_FUNCTION(KqAnim,customWithCallback,9,_SC("xa|ti|fiiii|fi|fc"))
_MEMBER_FUNCTION(KqAnim,animate,10,_SC("xa|ti|fiiii|fi|fc."))
_MEMBER_FUNCTION(KqAnim,custom,5,_SC("xa|ti|fii"))
_END_CLASS(KqAnim)