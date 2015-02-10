#ifndef DOMTimer_h
#define DOMTimer_h

#include "ScheduledAction.h"
#include <WTF/HashMap.h>

class DOMTimer {
public:
	static int GetTimeoutId() {
		static int lastUsedTimeoutId = 0;
		++lastUsedTimeoutId;
		// Avoid wraparound going negative on us.
		if (lastUsedTimeoutId <= 0)
			lastUsedTimeoutId = 1;
		return lastUsedTimeoutId;
	}

	DOMTimer() {}

	~DOMTimer() {
		WTF::HashMap<int, ScheduledAction*>::iterator it = m_times.begin();
		for (; it != m_times.end(); ++it)
			it->second->deref();
		m_times.clear();
	}

	int Install(ScheduledAction* action) {
		int timeoutid = GetTimeoutId();
		action->ref();
		m_times.insert(timeoutid, action);
		return timeoutid;
	}

	void RemoveById(int timeoutId) {
		if (0 == timeoutId)
			return;
			
		WTF::HashMap<int, ScheduledAction*>::iterator it = m_times.find(timeoutId);
		if (it == m_times.end())
			return;
		
		it->second->deref();
		m_times.remove(it);
	}

protected:
	WTF::HashMap<int, ScheduledAction*> m_times;
};

#endif // DOMTimer_h
