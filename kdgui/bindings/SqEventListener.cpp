
#include <UIlib.h>
#include <wtf/UtilHelp.h>
#include <wtf/RefCountedLeakCounter.h>

#include "SqEventListener.h"
#include "script/scripthelp/SquirrelBindingsUtils.h"
#include "dom/UINode.h"

#ifndef NDEBUG
static RefCountedLeakCounter SqEventListenerCounter(_SC("SqEventListener"));
#endif

SqEventListener::SqEventListener(HSQUIRRELVM v, HSQOBJECT listenerObj, HSQOBJECT listenerData, bool bUseCapture) 
	: EventListener(SqEventListenerType, bUseCapture) {
	m_listenerObj = listenerObj;
	m_listenerData = listenerData;
	m_v = v;

#ifndef NDEBUG
	SqEventListenerCounter.increment();
#endif
}

SqEventListener::~SqEventListener() {
	KDASSERT(OT_CLOSURE == m_listenerObj._type || OT_NULL == m_listenerObj._type);
	if (OT_CLOSURE == m_listenerObj._type && !m_bIsScriptDestroying) {
		KDASSERT (0 != m_v);

		sq_release(m_v, &m_listenerObj);
		m_listenerObj._type = OT_NULL;

		sq_release(m_v, &m_listenerData);      
	}

	m_fireCount = -1;
	m_refCount = -1;

#ifndef NDEBUG
	SqEventListenerCounter.decrement();
#endif
}

bool SqEventListener::Equal(const EventListener& other) const {
	if (m_type != other.GetType())
		return false;

	const SqEventListener* pOther = (const SqEventListener*)&other;	
	if (m_listenerObj._type == pOther->m_listenerObj._type &&
		m_listenerObj._unVal.pClosure == pOther->m_listenerObj._unVal.pClosure)
		return true;
	
	return false;
}

static SQInteger _eventToSqReleaseHook(SQUserPointer p, SQInteger size)
{
	SqPushEventStruct* data = (SqPushEventStruct*)p;
	*data->ref = 0;
	return 1;
}

static SqPushEventStruct* PushEventToSq(HSQUIRRELVM v, TEventUI* event, SQRELEASEHOOK hook)
{
	SqPushEventStruct* data = new SqPushEventStruct();
	data->event = event;
	KDASSERT(SbuCreateNativeClassInstance(v, _SC("KqEvt"), data, hook));

	return data;
}

class ProtectFireEvent { // 为了防止在派发事件的时候删除本节点
public:
	ProtectFireEvent(UINode* pTarget, UINode* pCurrentTarget) {
		CPageManager* manage = NULL;
		m_pTarget = pTarget;
		m_pCurrentTarget = pCurrentTarget;
		if (pTarget) {
			manage = pTarget->GetManager();
			//manage->PushFireEventNode(pTarget);
		} 
		
		if (pCurrentTarget) {
			manage = pCurrentTarget->GetManager();
			//manage->PushFireEventNode(pCurrentTarget);
		}

		if (!manage)
			return;
	}

	~ProtectFireEvent() {
		CPageManager* manage = NULL;
		if (m_pTarget) {
			manage = m_pTarget->GetManager();
			//manage->EraseFireEventNode(m_pTarget);
		} 
		
		if (m_pCurrentTarget) {
			manage = m_pCurrentTarget->GetManager();
			//manage->EraseFireEventNode(m_pCurrentTarget);
		}
	}

private:
	UINode* m_pTarget;
	UINode* m_pCurrentTarget;
};

void SqEventListener::FireEvent(TEventUI* evt) {
	m_fireCount++; // 有可能会重入到clear listener里
	ref(); // 之所以要引用加一，是为了防止在脚本中把本对象删除掉
	ProtectFireEvent protect(evt->pTarget, evt->pCurrentTarget);

	// 强制使用保存的虚拟机，而不是scriptExecutionContext携带的，这是因为可能发生了attach操作
	HSQUIRRELVM v = m_v;

	sq_addref(v, &m_listenerObj); // 之所以要引用加一，是为了防止在脚本中把本对象删除掉
	sq_addref(v, &m_listenerData);

	SQInteger top = sq_gettop(v); // 保存原始堆栈大小
	sq_pushobject(v, m_listenerObj);
	sq_pushroottable(v); // 松鼠的函数总是隐藏了一个this指针

	int refNum = 1;
	SqPushEventStruct* pushEvtStruct = PushEventToSq(v, evt, _eventToSqReleaseHook);
	pushEvtStruct->ref = &refNum;

	sq_pushobject(v, m_listenerData);

	if(SQ_FAILED(sq_call(v, 3, SQFalse, SQTrue)))
		KDASSERT(FALSE);
	KDASSERT (0 == refNum); // 如果脚本没释放了，说明被拷贝了，必须强制释放
						    // 暂时让这崩溃，因为出了函数的范围，必须保证是evt无效的
	delete pushEvtStruct;

	sq_release(v, &m_listenerData);

	int listenerRefConut = sq_getrefcount(v, &m_listenerObj);
	sq_release(v, &m_listenerObj);
	if (1 == listenerRefConut)  // 如果为1，说明脚本里调用了removeEventListener
		m_listenerObj._type = OT_NULL;

	sq_settop(v, top); // 还原堆栈

	KDASSERT(m_fireCount >= 1);
	m_fireCount--;

	deref(); // 既然能调用脚本，那管理器肯定准备完毕了
}
