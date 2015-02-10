#ifndef ImageNode_h
#define ImageNode_h

#include "style/NodeAttrDef.h"
#include "UINode.h"

class GraphicsContext;

class ImageNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	ImageNode();
	virtual ~ImageNode();

	virtual LPCTSTR GetClass() const {return _SC("image");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Image;}

	virtual IntRect BoundingRectInLocalCoordinates();
	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY, 
			eNRStyleWidth,
			eNRStyleHeight,

			eNRStyleSrc,

			eNRStyleVisualNodeCommAttr,

			eNRStyleEnd
		};
		return validAttrs;
	}

protected:
	void DrawClipFast(GraphicsContext* g, int image, const IntRect* imageClipRect);
	void DrawClipFastWithStretch(GraphicsContext* g, int image, FloatRect& clipRect, FloatRect& stretchRect);
};

#endif // ImageNode_h