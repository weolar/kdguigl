
class ListenersMgr {
public:
	typedef WTF::Vector<EventListener*, 4> EventListeners;
	EventListeners m_listeners[UIEVENT__LAST];

	bool HaveListenersByEventType(UIEventType Type) {
		return 0 != m_listeners[Type].size();
	}

	int DispatchEventToListeners(const UINode* n, TEventUI& event, BOOL bCapturePhase) {
		if (!m_listeners || UINode::eRunning != n->m_eState)
			return 0;

		size_t size = m_listeners[event.Type].size();
		if (0 == size)
			return 0;

		// TODO 如果在fire里注册或删除监听器，要处理这种情况
		EventListeners listenersCopy;
		for (size_t i = 0; i < size; ++i)
			listenersCopy.append(m_listeners[event.Type][i]);
		
		for (size_t i = 0; i < listenersCopy.size(); ++i) {
			EventListener* listener = listenersCopy[i];
			BOOL bDispatch = FALSE;
			if (!UHCanEventUseCapture(event.Type))
				bDispatch = TRUE;
			else {
				if (bCapturePhase == listener->IsUseCapture())
					bDispatch = TRUE;
				else if (!n->IsContainer()) // 如果不是容器，则不管是什么阶段的消息都派发
					bDispatch = TRUE;
			}

			if (bDispatch)
				listener->FireEvent(&event); // 如果不需求区分的事件，直接派发
		}

		return 0;
	}

	static bool IsListenerEqual(const UINode* n, UIEventType eventType, const EventListener& listenerA, const EventListener& listenerB) {
		if (!listenerA.Equal(listenerB))
			return false;

		if (listenerA.IsUseCapture() != listenerB.IsUseCapture() && UHCanEventUseCapture(eventType) && n->IsContainer())
			return false;
		return true;
	}

	bool AddEventListener(const UINode* n, UIEventType eventType, PassRefPtr<EventListener> eventListener) {
		EventListeners& it = m_listeners[eventType];

		for (size_t i = 0; i < it.size(); ++i) {
			if (!IsListenerEqual(n, eventType, *eventListener, *(it[i])))
				continue;

			return false;
		}

		eventListener->ref();
		it.push_back(eventListener.get());

		return true;
	}

	void RemoveEventListener(const UINode* n, CPageManager* pManager, UIEventType eventType, EventListener* eventListener) {
		EventListeners& it = m_listeners[eventType];

		for (size_t i = 0; i < it.size(); ++i) {
			if (IsListenerEqual(n, eventType, *eventListener, *(it[i]))) {
				it[i]->SetScriptIsDestroying(pManager->IsScriptDestroying());
				it[i]->deref();
				it.erase(i);
				return;
			}
		}
	}

	void ClearEventListener(CPageManager* pManager) {
		for (int eventType = UIEVENT__FIRST; eventType < UIEVENT__LAST; ++eventType) {
			EventListeners& it = m_listeners[eventType];

			for (size_t i = 0; i < it.size(); ++i) {
				EventListener* listeners = it[i];
				KDASSERT(listeners->fireCount() + 1 == listeners->refCount());
				listeners->SetScriptIsDestroying(pManager->IsScriptDestroying());
				listeners->deref();
			}
			it.clear();
		}
	}
};