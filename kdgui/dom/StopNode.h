#ifndef StopNode_h
#define StopNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

class StopNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	StopNode() {}

	virtual LPCTSTR GetClass() const {return _SC("stop");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Stop;}

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleStopColor,
			eNRStyleStopOpacity,
			eNRStyleOffset,
			eNRStyleEnd
		};
		return validAttrs;
	}

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {return NULL;}
};

#endif // StopNode_h