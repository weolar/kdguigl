#ifndef KqPropsPushHelp_h
#define KqPropsPushHelp_h

#include <wtf/RefCountedLeakCounter.h>
#include "script/include/squirrel.h"

enum KqPropsTypeVal {
	KqPropsTypeValString,
	KqPropsTypeValFloat,
};
// 辅助生成参数
struct KqPropsType {
	CStdString stringVal;
	float floatVal;
	KqPropsTypeVal type; // 0 - String, 1 - float
};

struct AnimPropPair {
	CStdString name;
	float value;
};

class KqPropsPushHelp {
	WTF_MAKE_FAST_ALLOCATED;

public:
	static KqPropsPushHelp* WINAPI create();

	void reserve(int size);

	KqPropsPushHelp* ref();
	KqPropsPushHelp* deref();

	KqPropsPushHelp* a(const SQChar* prop);
	KqPropsPushHelp* a(float prop);
	KqPropsPushHelp* a(const SQChar* prop, float value);
	KqPropsPushHelp* a(const SQChar* prop, const SQChar* value);

	WTF::Vector<KqPropsType> m_props;

protected:
	KqPropsPushHelp();
	~KqPropsPushHelp();

	int m_refCounted;

};
#define KQPUSH() (KqPropsPushHelp::create())

// bool KqGetParamFromSqArray(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher);
// bool KqGetParamFromSqTable(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher);

bool KqGetParamFromSq(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher);

#endif // KqPropsPushHelp_h