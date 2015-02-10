#ifndef RichEditBaseNode_h
#define RichEditBaseNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

enum NodeAttrDef;
class RenderRichEdit;
class RichEditHost;

class RichEditBaseNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	RichEditBaseNode();
	virtual ~RichEditBaseNode();

	virtual LPCTSTR GetClass() const {return L"richeditbase";}
	virtual UINodeEnum GetClassEnum() const {return eNE_RichEditBase;}

	// 单独对只有一个image节点的情况做剪裁优化
	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);
	virtual int Event(TEventUI& event, BOOL bCapturePhase);
	
	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY, 
			eNRStyleWidth,
			eNRStyleHeight,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
	
protected:
	void Init();
	RichEditHost* m_richeditRender;
};

#endif // RichEditBaseNode_h