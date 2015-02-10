
static bool WINAPI __FindControlByPointAndDispatchMsg(UINode* node, LPVOID pData, LPVOID pProcData) {
	FloatPoint* pPoint = static_cast<FloatPoint*>(pData);
	if (!node->BoundingRectAbsolute().contains((int)pPoint->x(), (int)pPoint->y()))
		return false;

	// 如果是容器类型，则需要派发消息，但不能接受焦点，也不能让查询机制在这里中断
	node->ref(); // 防止
	if (node->IsContainer()) { 
		TEventUI* event = (TEventUI*)pProcData;
		node->Event(*event, TRUE);
		node->deref();
		return false;
	}

	int ref = node->GetRef();
	node->deref();
	return (0 != ref); // 为0表示在派发消息的过程中此节点被干掉了
}

// 从根节点一路派发到最终节点,要发两遍。有些消息，不能边查询边派发，比如mouseout这种
// 找到目标节点后，需要从根节点一路发消息过去，并且每个父节点都要引用加一，防止被事件派发中的脚本代码销毁
static BOOL DispathMsgCaptureAndBubbling(UINode* node, const TEventUI& event) {
	TEventUI eventDummy = event;
	eventDummy.dwTimestamp = ::GetTickCount();
	WTF::Vector<UINode*> dispathedMsgNodes;
	BOOL bNotCallDefWindowProc = FALSE; // 只要有一层控件宣布调用defProc，则调用

	node->ref();
	UINode* parent = node;
	while (parent) { // 先收集
		dispathedMsgNodes.append(parent);
		parent = parent->GetParent();
	}

	eventDummy.pTarget = node;
	for (int i = (int)dispathedMsgNodes.size() - 1; i >= 0; --i) {
		if (i == 0)
			break;

		eventDummy.pCurrentTarget = dispathedMsgNodes[i];
		eventDummy.bNotHandle = FALSE;
		dispathedMsgNodes[i]->Event(eventDummy, TRUE);

		if (eventDummy.bNotCallDefWindowProc)
			bNotCallDefWindowProc = TRUE;

		if (eventDummy.bNotHandle)
			break;
	}

	for (size_t i = 0; i < dispathedMsgNodes.size(); ++i) {
		eventDummy.pCurrentTarget = dispathedMsgNodes[i];
		UINode* n = dispathedMsgNodes[i];
		eventDummy.bNotHandle = FALSE;

		n->Event(eventDummy, FALSE);

		if (eventDummy.bNotCallDefWindowProc)
			bNotCallDefWindowProc = TRUE;

		if (eventDummy.bNotHandle)
			break;
	}

	for (size_t i = 0; i < dispathedMsgNodes.size(); ++i)
		dispathedMsgNodes[i]->deref();

	return bNotCallDefWindowProc;
}

#ifdef _MSC_VER
static void _SetTipWnd(HWND hWnd, HWND hwndTooltip, UINode* pHover, TOOLINFO& toolTip) {
	CStdString sToolTip = pHover->GetToolTip();
	if( sToolTip.IsEmpty() ) return;

	::ZeroMemory(&toolTip, sizeof(TOOLINFO));
#if _WIN32_WINNT >= 0x0501
	toolTip.cbSize = sizeof(TOOLINFO) - sizeof(void*);
#else
	toolTip.cbSize = sizeof(TOOLINFO);
#endif  
	toolTip.uFlags = TTF_IDISHWND;
	toolTip.hwnd = hWnd;
	toolTip.uId = (UINT) hWnd;
	toolTip.hinst = NULL;
	toolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
	toolTip.rect = pHover->BoundingRectAbsolute();
	if( hwndTooltip == NULL ) {
		hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, NULL, NULL);
		::SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &toolTip);
	}
	::SendMessage(hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &toolTip);
	::SendMessage(hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &toolTip);
}
#endif

class MessageMgr {
private:
	CPageManager* m_pManager;
	WTF::Vector<UINode*, 4> m_fireEventingNodes;
	//
	RefPtr<UINode> m_pFocus;
	RefPtr<UINode> m_pEventHover;
	RefPtr<UINode> m_pEventClick;
	RefPtr<UINode> m_pEventKey;
	//
	POINT m_ptLastMousePos;
	SIZE m_szMinWindow;
	//UINT m_uMsgMouseWheel;
	bool m_bFirstLayout;
	bool m_bFocusNeeded;
	bool m_bMouseTracking;
	bool m_bMouseIn;

	bool m_bIsDraggableRegionNcHitTest;
	IntPoint m_lastPosForDrag;

	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;

public:
	MessageMgr (CPageManager* pManager)
		: m_pManager(pManager)
		, m_bFirstLayout(true)
		, m_bFocusNeeded(false)
		, m_bMouseTracking(false) 
		, m_bIsDraggableRegionNcHitTest(false)
		, m_bMouseIn(false)
		, m_hwndTooltip(NULL) {
		m_szMinWindow.cx = 140;
		m_szMinWindow.cy = 200;
		m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
		//m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
		memset(&m_ToolTip, 0, sizeof(TOOLINFO));
	}

	void Init() {
		m_bFirstLayout = true;
		m_bFocusNeeded = true;

		ResetNode();
	}

	void Uninit() {
		ResetNode();
	}

	void ResetNode() {
		m_pFocus = NULL;
		m_pEventKey = NULL;
		m_pEventHover = NULL;
		m_pEventClick = NULL;
	}

	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		m_bMouseTracking = false;

		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		TEventUI event;
		event.ptMouse = pt;
		event.Type = UIEVENT_MOUSEENTER;
		event.dwTimestamp = ::GetTickCount();

		RefPtr<UINode> pHover = FindControlByPointButNoDispatchMsg(FloatPoint(pt), &event);
		if(pHover.get() == NULL) return 0;
#if 0
		if(m_pEventHover != NULL)
			DispathMsgCaptureAndBubbling(m_pEventHover.get(), event);
#endif

#ifdef _MSC_VER
		_SetTipWnd(m_pManager->GetHWND(), m_hwndTooltip, pHover.get(), m_ToolTip);
#endif
		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if(m_hwndTooltip) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		
		TEventUI event;
		event.Type = UIEVENT_MOUSELEAVE;
		event.button = 0;
		
		if(m_bMouseTracking) {
			//OnMouseMove(WM_MOUSEMOVE, 0, (LPARAM)-1, bHandled);
			if (m_pEventHover)
				DispathMsgCaptureAndBubbling(m_pEventHover.get(), event);
			DispatchToDocNode(event);
		}
		m_bMouseTracking = false;

		event.Type = UIEVENT_MOUSEUP;
		if (m_pEventClick) 
			DispathMsgCaptureAndBubbling(m_pEventClick.get(), event);
		m_pEventClick = NULL;
		m_pEventHover = NULL;

		return 0;
	}

	void DispatchToDocNode(const TEventUI& evt) {
		TEventUI eventDummy = evt;

		UIEventType oldType = evt.Type;
		// 触屏里，会先收到mousedwon，而不是mousover
		if (!m_bMouseIn && UIEVENT_MOUSEENTER != evt.Type) {
			eventDummy.Type = UIEVENT_MOUSEENTER;
			DoDispatchToDocNode(eventDummy);
		}

		m_bMouseIn = UIEVENT_MOUSELEAVE != oldType;
		
		eventDummy.Type = oldType;
		DoDispatchToDocNode(eventDummy);
	}

	void DoDispatchToDocNode(TEventUI& evt) {
		UINode* docNode = m_pManager->GetDocNodeTemporary();
		if (!docNode->HaveListenersByEventType(evt.Type))
			return;
		
		docNode->Event(evt, true);
		docNode->Event(evt, false);
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if( !m_bMouseTracking ) {
			TRACKMOUSEEVENT tme = {0};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = m_pManager->GetHWND();
			tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			_TrackMouseEvent(&tme);
			m_bMouseTracking = true;
		}

		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if (-1 == lParam) {
			::GetCursorPos(&pt);
			::ScreenToClient(m_pManager->GetHWND(), &pt);
		}
		m_ptLastMousePos = pt;

		TEventUI event;
		event.ptMouse = pt;
		event.Type = UIEVENT_MOUSEMOVE;
		event.dwTimestamp = ::GetTickCount();

		DispatchToDocNode(event);
		RefPtr<UINode> pNewHover = FindControlByPointButNoDispatchMsg(FloatPoint(pt), &event);

		if( pNewHover != m_pEventHover && m_pEventHover.get() != NULL ) {
			event.Type = UIEVENT_MOUSELEAVE;
			DispathMsgCaptureAndBubbling(m_pEventHover.get(), event);
			RefPtr<UINode> protect = m_pEventHover; // 在析构任何节点的时候，会检测是否为m_pEventHover等，所以这里为了避免检测失败而做
			m_pEventHover = NULL;
			if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		}

		if( pNewHover != m_pEventHover && pNewHover.get() != NULL ) {
			event.Type = UIEVENT_MOUSEENTER;
			DispathMsgCaptureAndBubbling(pNewHover.get(), event);
			RefPtr<UINode> protect = m_pEventHover;
			m_pEventHover = pNewHover;
		}

		if( m_pEventClick.get() != NULL ) {
			event.Type = UIEVENT_MOUSEMOVE;
			DispathMsgCaptureAndBubbling(m_pEventClick.get(), event);
		} else if( pNewHover.get() != NULL ) {
			event.Type = UIEVENT_MOUSEMOVE;
			DispathMsgCaptureAndBubbling(pNewHover.get(), event);
		}

		return 0;
	}

	LRESULT OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		::SetFocus(m_pManager->GetHWND());

		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		m_ptLastMousePos = pt;
		TEventUI event;
		event.Type = UIEVENT_MOUSEDOWN;
		if (uMsg == WM_LBUTTONDOWN)
			event.button = 0;
		else
			event.button = 2;

		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = pt;
		event.wKeyState = wParam;
		DispatchToDocNode(event);

		m_bIsDraggableRegionNcHitTest = false;

		RefPtr<UINode> pControl = FindControlByPointButNoDispatchMsg(FloatPoint(pt), &event);
		if( pControl.get() == NULL )
			return 0;

		RefPtr<UINode> protect = m_pEventClick;
		m_pEventClick = pControl;
		pControl->SetFocus();
		
		DispathMsgCaptureAndBubbling(pControl.get(), event);
		if (m_bIsDraggableRegionNcHitTest) {
			::PostMessage(m_pManager->GetHWND(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
			m_lastPosForDrag = IntPoint(pt.x, pt.y);
		} else
			::SetCapture(m_pManager->GetHWND());

		return 0;
	}

	LRESULT OnMouseUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		m_ptLastMousePos = pt;

		TEventUI event;
		event.Type = UIEVENT_MOUSEUP;
		if (uMsg == WM_LBUTTONUP) {
			event.button = 0;
			::ReleaseCapture();
		} else
			event.button = 2;

		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = pt;
		event.wKeyState = wParam;
		DispatchToDocNode(event);

		RefPtr<UINode> pControl = FindControlByPointButNoDispatchMsg(FloatPoint(pt), &event);
		if(pControl.get() == NULL)
			return 0;

		DispathMsgCaptureAndBubbling(pControl.get(), event);

		if( m_pEventClick == NULL )
			return 0;

		event.Type = UIEVENT_CLICK;
		if (m_pEventClick == pControl)
			DispathMsgCaptureAndBubbling(m_pEventClick.get(), event);
		RefPtr<UINode> protect = m_pEventClick;
		m_pEventClick = NULL;

		::ReleaseCapture();

		return 0;
	}

	UINode* GetFocus() const {
		return m_pFocus.get();
	}

	void SetFocus(UINode* pControl) {
		if(pControl == m_pFocus.get())
			return;

		if(m_pFocus.get()) {
			TEventUI event;
			event.Type = UIEVENT_KILLFOCUS;
			DispathMsgCaptureAndBubbling(m_pFocus.get(), event);
			m_pFocus = NULL;
		}

		if(pControl) {
			m_pFocus = pControl;
			TEventUI event;
			event.Type = UIEVENT_SETFOCUS;
			DispathMsgCaptureAndBubbling(pControl, event);
		}
	}

	PassRefPtr<UINode> FindControlByPointButNoDispatchMsg(FloatPoint pt, const TEventUI* event) const {
		RefPtr<UINode> node = m_pManager->GetRootTemporary()->FindControl(NULL, &pt, UIFIND_VISIBLE|UIFIND_HITTEST, (LPVOID)event);
		if (!node.get())
			return NULL;

		return node;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		UINT cx = LOWORD(lParam);
		UINT cy = HIWORD(lParam); // 这个大小是客户区的，我们下面要的是整个窗口的

		if (0 >= cx || 0 >= cy)
			return 0;

		RECT rtWnd;
		BOOL b = ::GetWindowRect(m_pManager->GetHWND(), &rtWnd);

		IntRect rc(0, 0, rtWnd.right - rtWnd.left, rtWnd.bottom - rtWnd.top);
		m_pManager->GetPaintMgr()->SetClientRectAndInvalideta(rc);

		TEventUI event;
		event.Type = UIEVENT_WINDOWSIZE;
		DispatchToDocNode(event);

		if(m_pFocus.get())
			DispathMsgCaptureAndBubbling(m_pFocus.get(), event);

		return 0;
	}

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		TEventUI event;
		event.Type = UIEVENT_MOUSEWHEEL;
		event.wheelDelta = (int)(short)HIWORD(wParam);
		DispatchToDocNode(event);

		if(NULL == m_pFocus.get()) {
			if (m_pManager->GetRootTemporary()->HaveListenersByEventType(UIEVENT_MOUSEWHEEL))
				DispathMsgCaptureAndBubbling(m_pManager->GetRootTemporary(), event);
		} else
			DispathMsgCaptureAndBubbling(m_pFocus.get(), event);

		return 0;
	}

	// 这条消息会在WM_NCLBUTTONDOWN后收到，要做点特殊处理~详情见：http://hi.baidu.com/killdbg/item/03a64ea738382d038919d3f0
	LRESULT OnCaptureChanged(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if (!m_bIsDraggableRegionNcHitTest)
			return 0;

		::ReleaseCapture();
		m_bIsDraggableRegionNcHitTest = false;

		lParam = MAKELONG(m_lastPosForDrag.x(), m_lastPosForDrag.y());
		OnMouseUp(WM_LBUTTONUP, 0, lParam, bHandled);

		return 0;
	}

	LRESULT OnKillFocus(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {

	}

	int OnSetFocus(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return OnInputMsg(message, wParam, lParam, bHandled);
	}

	int OnInputMsg(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if (!m_pFocus.get())
			return 0;

		if (WM_KEYDOWN == message)
			message = message;
		
		TEventUI event;
		event.Type = UIEVENT_SYSKEY;
		event.uMsg = message;
		event.wParam = wParam;
		event.lParam = lParam;
		DispatchToDocNode(event);
		bHandled = !(DispathMsgCaptureAndBubbling(m_pFocus.get(), event));

		return 0;
	}

	void SetIsDraggableRegionNcHitTest() {
		m_bIsDraggableRegionNcHitTest = true;
	}

	void CheckForReapObjects(UINode* pControl) {
		KDASSERT(pControl != m_pEventKey.get());
		KDASSERT(pControl != m_pEventHover.get());
		KDASSERT(pControl != m_pEventClick.get());
		KDASSERT(pControl != m_pFocus.get());
	}

	BEGIN_MSG_MAP(MessageMgr)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		
		MESSAGE_RANGE_HANDLER(WM_KEYDOWN, WM_CHAR, OnInputMsg)
		MESSAGE_RANGE_HANDLER(WM_IME_SETCONTEXT, WM_IME_KEYUP, OnInputMsg)
		MESSAGE_RANGE_HANDLER(WM_IME_STARTCOMPOSITION, WM_IME_KEYLAST, OnInputMsg)
	END_MSG_MAP()
};