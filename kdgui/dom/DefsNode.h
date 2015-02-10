#ifndef DefsNode_h
#define DefsNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

enum NodeAttrDef;

class DefsNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	DefsNode() {}
	virtual ~DefsNode() {}

	virtual LPCTSTR GetClass() const {return _SC("defs");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Defs;}
	virtual BOOL IsContainer() const {return TRUE;}

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid) {}
	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {return NULL;}
};

#endif // DefsNode_h