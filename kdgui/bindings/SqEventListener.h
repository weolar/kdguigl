#ifndef SqEventListener_h
#define SqEventListener_h

#include "dom/EventListener.h"
#include "script/include/squirrel.h"

class SqPushEventStruct {
	WTF_MAKE_FAST_ALLOCATED;
public:
	TEventUI* event;
	int* ref;
};

class SqEventListener : public EventListener {
public:
	static PassRefPtr<SqEventListener>
		Create(HSQUIRRELVM v, HSQOBJECT listenerObj, HSQOBJECT listenerData, bool bUseCapture) {
		return adoptRef(new SqEventListener(v, listenerObj, listenerData, bUseCapture));
	}

	virtual ~SqEventListener();
	virtual bool Equal(const EventListener&) const;
	virtual void FireEvent(TEventUI*);

protected:
	SqEventListener(HSQUIRRELVM v, HSQOBJECT listenerObj, HSQOBJECT listenerData, bool bUseCapture);

	HSQOBJECT m_listenerObj;
	HSQOBJECT m_listenerData;
	HSQUIRRELVM m_v;
};

#endif // SqEventListener_h