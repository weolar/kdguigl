
#include <UIlib.h>
#include "ScheduledAction.h"
#include "DOMTimer.h"

ScheduledAction::ScheduledAction(HSQUIRRELVM v, HSQOBJECT* function, int timeout, bool singleShot, ThreadTimers* threadTimers, DOMTimer* pDOMTimer)
	: m_function(*function)
	, m_timer(this, &ScheduledAction::Fire, threadTimers)
	, refCount(0)
	, m_DOMTimer(pDOMTimer)
	, m_bSingleShot(singleShot)
	, m_timerId(0)
	, m_v(v) {
	sq_addref(v, &m_function);
	Start(timeout, singleShot);
}

ScheduledAction::~ScheduledAction() {
	sq_release(m_v, &m_function);
}

void ScheduledAction::deref() {
	--refCount;
	if (0 == refCount)
		delete this;
}

void ScheduledAction::Fire(Timer<ScheduledAction>*) {
	HSQUIRRELVM v = m_v;
	HSQOBJECT functionOfCloser = m_function;
	sq_addref(v, &functionOfCloser); // 之所以要引用加一，是为了防止在脚本中把本对象删除掉

	SQInteger top = sq_gettop(v); // 保存原始堆栈大小
	sq_pushobject(v, functionOfCloser);
	sq_pushroottable(v); // 松鼠的函数总是隐藏了一个this指针

	ref();
	// 注意，在脚本里有可能调用clearInterval，导致本this被delete，所以在call之后不允许访问成员变量了
	sq_call(v, 1, SQFalse, SQTrue);
	
	int listenerRefConut = sq_getrefcount(v, &functionOfCloser);
	sq_release(v, &functionOfCloser);

	sq_settop(v, top); // 还原堆栈

	if (m_bSingleShot) // 如果是执行一次，则干掉自己
		m_DOMTimer->RemoveById(m_timerId);
	deref();
}

void ScheduledAction::Start(int timeout, bool singleShot) {
	double intervalMilliseconds = timeout/1000.0;
	if (singleShot)
		m_timer.startOneShot(intervalMilliseconds);
	else
		m_timer.startRepeating(intervalMilliseconds);
}