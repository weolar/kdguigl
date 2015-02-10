#ifndef ScheduledAction_h
#define ScheduledAction_h

#include "script/include/squirrel.h"
#include <wtf/Timer.h>

class DOMTimer;
class ThreadTimers;

class ScheduledAction {
public:
	ScheduledAction(HSQUIRRELVM v, HSQOBJECT* function, int timeout, bool singleShot, ThreadTimers* threadTimers, DOMTimer* pDOMTimer);
	~ScheduledAction();
	void Fire(Timer<ScheduledAction>*);

	void ref() {refCount++;}
	void deref();

	void SetTimerId(int timerId) {m_timerId = timerId;}

protected:
	void Start(int timeout, bool singleShot);

	int refCount;

	HSQOBJECT m_function;
	HSQUIRRELVM m_v;

	Timer<ScheduledAction> m_timer;

	int m_bSingleShot;
	DOMTimer* m_DOMTimer;

	int m_timerId;
};

#endif // ScheduledAction_h