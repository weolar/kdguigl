#ifndef LinearGradientNode_h
#define LinearGradientNode_h

#include "GradientNodeComm.h"

enum NodeAttrDef;

class LinearGradientNode : public GradientNodeComm {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("linearGradient");}
	virtual UINodeEnum GetClassEnum() const {return eNE_LinearGradient;}

	virtual GradientData* CollectGradientArgByNode(UINode* owner);

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX1,
			eNRStyleY1, 
			eNRStyleX2,
			eNRStyleY2,
		};
		return validAttrs;
	}
};

#endif // LinearGradientNode_h