
#ifndef ResCache_h
#define ResCache_h

#include <WTF/HashMap.h>
#include <vector>

class CPageManager;
struct NVGcontext;

enum ResItemType {
	eRITImage,
};

struct ResItem {
	ResItem() {

	}

	ResItem(ResItemType type_, void* resData_, UINode* owner) {
		type = type_;
		resData = resData_;
		owners.set(owner, TRUE);
	}

	ResItemType type;
	void* resData;
	WTF::HashMap<UINode*, BOOL> owners; // 资源被哪些节点占有
};

class ResCache {
public:
	ResCache();

	static void SetDisable();

	static ResCache* GetCache();

	bool CheckNoStoreWhenResLoad(CStdString& src);
	int GetResBySrc(NVGcontext* ctx, const CStdString& pSrc, UINode* owner);

protected:
	WTF::HashMap<ULONG_PTR, ResItem> m_resMap;
};

#endif // ResCache_h