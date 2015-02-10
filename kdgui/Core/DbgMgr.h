
// 界面引擎是服务端，spy是客户端
UINT c_DbgServiceMsg = ::RegisterWindowMessage(L"DbgServiceMsg{CDA634FC-3B6F-49e6-AD70-0A91F142AE75}");
UINT c_DbgClientMsg = ::RegisterWindowMessage(L"DbgClientMsg{CDA634FC-3B6F-49e6-AD70-0A91F142AE75}");

#define WM_SHOWDEBUGNODEDATA (WM_APP + 0xcccc)
#define WM_HIGHTLIGHTRING (WM_APP + 0xcccd)
#define WM_BROADCAST_SERVICE_MSG (WM_APP + 0xccce)
#define WM_ON_RECIVE_CLINET_HWND (WM_APP + 0xcccf)

class DbgMgr {
public:
	DbgMgr (CPageManager* pManager) : m_pManager(pManager) {}

	void Init() {}

	LRESULT OnDumpNode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		DumpNode();
		return 0;
	}

	LRESULT OnShowHightLightRing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		UINode* n = (UINode*)wParam;

		if (m_pManager->GetPaintMgr())
			m_pManager->GetPaintMgr()->HighLightRing(n);
		return 0;
	}

	LRESULT OnBroadcastServiceHWNDMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		::PostMessage(HWND_BROADCAST, c_DbgServiceMsg, (WPARAM)m_pManager->GetHWND(), 0);
		return 0;
	}

	LRESULT OnReciveClientHWND(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		::PostMessage(HWND_BROADCAST, c_DbgServiceMsg, (WPARAM)m_pManager->GetHWND(), 0);
		return 0;
	}

	void DumpNode() {
		CStdString output;
		if (m_pManager->GetRootTemporary())
			output = m_pManager->GetRootTemporary()->Dump();

		if (!m_hClientHWND)
			return;
		
		COPYDATASTRUCT copyData = {0};
		copyData.dwData = WM_SHOWDEBUGNODEDATA;
		copyData.cbData = output.GetLength() * sizeof(WCHAR);
		copyData.lpData = output.GetBuffer();
		::SendMessage(m_hClientHWND, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&copyData);
	}

	LRESULT OnDbgSaveClinetHWND(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		m_hClientHWND = (HWND)wParam;

		DumpNode();
		return 0;
	}

private:
	BEGIN_MSG_MAP(DbgMgr)
		MESSAGE_HANDLER(c_DbgClientMsg, OnDbgSaveClinetHWND)

		MESSAGE_HANDLER(WM_SHOWDEBUGNODEDATA, OnDumpNode)
		MESSAGE_HANDLER(WM_HIGHTLIGHTRING, OnShowHightLightRing)

		MESSAGE_HANDLER(WM_BROADCAST_SERVICE_MSG, OnBroadcastServiceHWNDMsg)
		MESSAGE_HANDLER(WM_ON_RECIVE_CLINET_HWND, OnReciveClientHWND)
		
	END_MSG_MAP()

	CPageManager* m_pManager;
	HWND m_hClientHWND;
};