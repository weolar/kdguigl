
#ifndef NodeAttrDefHash_h
#define NodeAttrDefHash_h

#include "NodeAttrDef.h"
#include <wtf/HashFunctions.h>
#include <wtf/HashTraits.h>

// The empty value is (0, INT_MIN), the deleted value is (INT_MIN, 0)
namespace WTF {

struct NodeAttrDefHash {
	static unsigned hash(const NodeAttrDef& p) { return (unsigned)(p); }
	static bool equal(const NodeAttrDef& a, const NodeAttrDef& b) { return a == b; }
	static const bool safeToCompareToEmptyOrDeleted = true;
};
template<> struct HashTraits<NodeAttrDef> : GenericHashTraits<NodeAttrDef> {
	static const bool needsDestruction = false;
	static NodeAttrDef emptyValue() { return eNRStyleEnd; }

	static void constructDeletedValue(NodeAttrDef& slot) { slot = eNRStyleEnd; }
	static bool isDeletedValue(const NodeAttrDef& slot) { return slot == eNRStyleEnd; }
};
template<> struct DefaultHash<NodeAttrDef> {
	typedef NodeAttrDefHash Hash;
};

} // WTF

#endif // NodeAttrDefHash_h