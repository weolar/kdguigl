
#ifndef EventListener_h
#define EventListener_h

#include <wtf/RefCounted.h>

class EventListener /*: public RefCounted<EventListener>*/ {
public:
	enum Type {
		SqEventListenerType, 
	};

	void ref() {
		++m_refCount;
	}

	int refCount() const {
		return m_refCount;
	}

	void deref() {
		--m_refCount;
		if (m_refCount <= 0)
			delete this;
	}

	int fireCount() const {
		return m_fireCount;
	}

	virtual ~EventListener() { }
	
	virtual bool Equal(const EventListener&) const = 0;
	virtual void FireEvent(TEventUI*) = 0;

	Type GetType() const { return m_type; }

	BOOL IsUseCapture() const {return m_bUseCapture;}
	void SetScriptIsDestroying(bool b) {m_bIsScriptDestroying = b;}

protected:
	EventListener(Type type, bool bUseCapture) 
		: m_type(type) 
		, m_refCount(1)
		, m_fireCount(0)
		, m_bIsScriptDestroying(false)
	, m_bUseCapture(bUseCapture) {}

	int m_refCount;
	int m_fireCount; // 在fire的时候，有可能会重入到clear，所以需要标记一下
	Type m_type;
	BOOL m_bUseCapture;
	bool m_bIsScriptDestroying;
};

#endif // EventListener_h