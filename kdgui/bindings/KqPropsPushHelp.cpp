#include "UIlib.h"
#include "KqPropsPushHelp.h"

#ifndef NDEBUG
static RefCountedLeakCounter KqPropsPushHelpCounter(_SC("KqPropsPushHelp"));
#endif

KqPropsPushHelp::KqPropsPushHelp()
: m_refCounted(0)
{
	ref();
#ifndef NDEBUG
	KqPropsPushHelpCounter.increment();
#endif
	//m_props.resize(4);
}

KqPropsPushHelp::~KqPropsPushHelp()
{
#ifndef NDEBUG
	KqPropsPushHelpCounter.decrement();
#endif
}

void KqPropsPushHelp::reserve(int size)
{
	//m_props.resize(size);
}

KqPropsPushHelp* KqPropsPushHelp::create()
{
	return (new KqPropsPushHelp());
}

KqPropsPushHelp* KqPropsPushHelp::a(const SQChar* prop)
{
	KqPropsType propsType;
	propsType.type = KqPropsTypeValString;
	propsType.stringVal = prop;

	m_props.push_back(propsType);
	return this;
}

KqPropsPushHelp* KqPropsPushHelp::a(float prop)
{
	KqPropsType propsType;
	propsType.type = KqPropsTypeValFloat;
	propsType.floatVal = prop;

	m_props.push_back(propsType);
	return this;
}

KqPropsPushHelp* KqPropsPushHelp::a(const SQChar* prop, float value)
{
	a(prop)->a(value);
	return this;
}

KqPropsPushHelp* KqPropsPushHelp::a(const SQChar* prop, const SQChar* value)
{
	a(prop)->a(value);
	return this;
}

KqPropsPushHelp* KqPropsPushHelp::ref()
{
	m_refCounted++;
	return this;
}

KqPropsPushHelp* KqPropsPushHelp::deref()
{
	m_refCounted--;
	if (0 == m_refCounted) {
		delete (this);
		return 0;
	}

	return this;
}

//////////////////////////////////////////////////////////////////////////

static bool GetParamFromSqArray(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher) {
	SQInteger    i=1;
	SQRESULT     result = 0;
	SQObjectType type;

	const SQChar* attrName = 0;
	const SQChar* attrValStr = 0;
	SQInteger     attrValInt = 0;
	SQFloat       attrValFloat = 0;

	type = sq_gettype(v, idx);
	ASSERT(OT_ARRAY == type);

	SQInteger nLen = sq_getsize(v, idx);
	ASSERT(0 == nLen%2);

	for (i = 0; i < nLen; ++i) {
		sq_pushinteger(v, i);

		result = sq_get(v, idx);
		if(!SQ_SUCCEEDED(result))
			break;

		type = sq_gettype(v, -1);
		switch(type) {
		case OT_INTEGER:
			sq_getinteger(v, -1, &attrValInt);
			break;
		case OT_FLOAT:
			sq_getfloat(v, -1, &attrValFloat);
			break;
		case OT_STRING:
			if (0 == i%2) { // 如果是偶数，表示属性名
				sq_getstring(v, -1, &attrName);
				break;
			}
			sq_getstring(v, -1, &attrValStr);
			break;
		default:
			DebugBreak();
		}
		if (1 == i%2) { // 如果是奇数，表示可以push了
			switch(type)
			{
			case OT_INTEGER:
				pusher->a(attrName, float(attrValInt));
				break;
			case OT_FLOAT:
				pusher->a(attrName, attrValFloat);
				break;
			case OT_STRING:
				pusher->a(attrName, attrValStr);
				break;
			}
			attrName = 0;
			attrValStr = 0;
			attrValInt = 0;
			attrValFloat = 0;
		}
		sq_pop(v, 1);
	}

	return true;
}

static bool GetParamFromSqTable(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher) {
	const SQChar* attrName = 0;
	const SQChar* attrValStr = 0;
	SQInteger     attrValInt = 0;
	SQFloat       attrValFloat = 0;
	SQObjectType  type = sq_gettype(v, idx);

	if (OT_TABLE != type) {
		return false;}

	sq_pushnull(v);  //null iterator
	while(SQ_SUCCEEDED(sq_next(v, idx))) {
		// here -1 is the value and -2 is the key
		// os << squirrel2string(v, -2) << " => " << squirrel2string(v, -1);
		if (OT_STRING != sq_gettype(v, -2)) {
			break;}

		if (!SQ_SUCCEEDED(sq_getstring(v, -2, &attrName))) {
			break;}

		type = sq_gettype(v, -1);
		switch(type) {
		case OT_INTEGER:
			sq_getinteger(v, -1, &attrValInt);
			pusher->a(attrName, float(attrValInt));
			break;
		case OT_FLOAT:
			sq_getfloat(v, -1, &attrValFloat);
			pusher->a(attrName, attrValFloat);
			break;
		case OT_STRING:
			sq_getstring(v, -1, &attrValStr);
			pusher->a(attrName, attrValStr);
			break;
		default:
			DebugBreak();
		}
		attrName = 0;
		attrValStr = 0;
		attrValInt = 0;
		attrValFloat = 0;
		sq_pop(v,2); // pops key and val before the nex iteration
	}
	sq_pop(v, 1);

	return true;
}

bool KqGetParamFromSq(HSQUIRRELVM v, int idx, KqPropsPushHelp* pusher) {
	if (OT_TABLE == sq_gettype(v, idx)) {
		return GetParamFromSqTable(v, idx, pusher);
	} else if (OT_ARRAY == sq_gettype(v, idx)) {
		return GetParamFromSqArray(v, idx, pusher);
	}
	DebugBreak();
	return false;
}