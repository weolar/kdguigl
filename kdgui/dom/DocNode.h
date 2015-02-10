#ifndef DocNode_h
#define DocNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

enum NodeAttrDef;

// 文档节点和根节点的区别在于，文档节点表示窗口，根节点表示所有元素的根
class DocNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;

public:
	DocNode() {}
	virtual ~DocNode() {}

	virtual LPCTSTR GetClass() const {return _SC("document");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Doc;}

	virtual BOOL IsContainer() const {return TRUE;}

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {
		for(int it = m_children.size() - 1; it >= 0; --it) {
			UINode* n = static_cast<UINode*>(m_children[it]);
			RefPtr<UINode> ret = n->FindControl(Proc, pData, uFlags, pProcData);
			if (ret.get())
				return ret;
		}

		if (!pProcData)
			return NULL;

		const TEventUI* event = (const TEventUI*)pProcData;
		// 防止鼠标滑出窗口，还能在mouseleave中触发命中root node
		if (UIEVENT_MOUSEENTER == event->Type) 
			return NULL;
		
		if (HaveListenersByEventType(event->Type))
			return this;
		
		return NULL;
	}

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {eNRStyleEnd};
		return validAttrs;
	}
};

#endif // DocNode_h