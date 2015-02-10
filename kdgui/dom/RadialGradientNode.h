#ifndef RadialGradientNode_h
#define RadialGradientNode_h

#include "GradientNodeComm.h"

enum NodeAttrDef;

class RadialGradientNode : public GradientNodeComm {
public:
	virtual LPCTSTR GetClass() const {return _SC("radialGradient");}
	virtual UINodeEnum GetClassEnum() const {return eNE_RadialGradient;}

	virtual GradientData* CollectGradientArgByNode(UINode* owner);

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleR,
			eNRStyleCx,
			eNRStyleCy,
			eNRStyleFx,
			eNRStyleFy,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

#endif // RadialGradientNode_h