#ifndef GNode_h
#define GNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

enum NodeAttrDef;

class GNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	GNode();
	virtual ~GNode();

	virtual LPCTSTR GetClass() const {return _SC("g");}
	virtual UINodeEnum GetClassEnum() const {return eNE_G;}
	virtual BOOL IsContainer() const {return TRUE;}

	// 单独对只有一个image节点的情况做剪裁优化
	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);
	
	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const;

	const IntRect* GetFastImageClipRect () const {return m_fastImageClipRect;}

protected:
	bool CanFastDrawImage();
	void PaintFastImage(GraphicsContext* g, const IntRect &rcInvalid);
	IntRect* m_fastImageClipRect;
};

#endif // GNode_h