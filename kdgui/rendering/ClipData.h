#ifndef ClipData_h
#define ClipData_h

#include <wtf/RefCounted.h>

class KdPath;

// 本类和ClipAttrData的区别是，那个是从属性里取出来的，还要经过本类包装一层才能返回给外面
// 不直接返回m_path的原因是，有些剪切域是overflow，是被new出来的，用完了需要删除，但别的不需要
class ClipData : public RefCounted<ClipData> {
	WTF_MAKE_FAST_ALLOCATED;
public:
	ClipData(KdPath* path, bool bDeleteIfUninit) 
		: m_path(path)
		, m_bDeleteIfUninit(bDeleteIfUninit) {}
	~ClipData();

	const KdPath* Path() {return m_path;}
	
protected:
	bool m_bDeleteIfUninit;
	KdPath* m_path;
};

#endif // ClipData_h