class IdMgr {
	WTF::HashMap<DWORD, UINode*> m_idMap;

public:
	void Init() {
	}

	void AddToIdMap(const SQChar* id, UINode* n) {
		if (!id)
			return;

		CStdString Id(id);
		Id.MakeLower();
		if (0 >= Id.GetLength())
			return;

		UINT idHash = UHGetNameHash(Id.GetBuffer());
		WTF::HashMap<DWORD, UINode*>::iterator it = m_idMap.find(idHash);
		if (it != m_idMap.end() && it->second != n) { // 如果id重名了，就不设置，兼容老kdgui2
			OutputDebugString(_SC("以下节点id有重复："));
			OutputDebugString(id);
			KDASSERT(FALSE);

			// kdgui2对重名的逻辑是，两个节点重名，则GetNodeById获取的是靠外层的节点
			return;
		}
		m_idMap.set(idHash, n);
	}

	void RemoveIdMapByNode(UINode* n) {
		bool bLoop = false;
		do {
			bLoop = false;
			WTF::HashMap<DWORD, UINode*>::iterator it;
			for (it = m_idMap.begin(); it != m_idMap.end(); ++it) {
				if (n == it->second) {
					bLoop = true;
					m_idMap.remove(it);
					break;
				}
			}
		} while (bLoop);
	}

	UINode* GetNodeByID(const CStdString& id) {
		if (id.IsEmpty())
			return NULL;

		CStdString lowID = id;
		lowID.MakeLower();
		UINT idHash = UHGetNameHash(lowID.GetString());
		WTF::HashMap<DWORD, UINode*>::iterator it = m_idMap.find(idHash);
		if (it == m_idMap.end())
			return NULL;

		it->second->ref();
		return it->second;
	}
};