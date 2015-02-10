#ifndef ClipPathNode_h
#define ClipPathNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"
#include "rendering/RenderSVGResource.h"

class KdPath;

class ClipPathNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	ClipPathNode();
	virtual ~ClipPathNode();
	virtual LPCTSTR GetClass() const {return _SC("clipPath");}
	virtual UINodeEnum GetClassEnum() const {return eNE_ClipPath;}

	virtual void SetManager(CPageManager* pManager);

	virtual BOOL IsContainer() const {return TRUE;}

	GradientData* CollectGradientArgByNode(UINode* owner);

// 	virtual void OnChildSetAttr();
// 	virtual bool AppendChild(UINode* pControl);
// 	virtual bool RemoveChild(UINode* pControl);
	virtual void OnChildSetAttrBegin();
	virtual void OnChildSetAttrEnd();

	virtual bool SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue);
// 	virtual void OnChildInvalidateAbsoluteRect(const IntRect& rc, bool bForce);
// 	virtual void OnChildInvalidateRelativeRect(const IntRect& rc, bool bForce);

	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret);

	const KdPath* CollectPathByChildren(UINode* owner);

	virtual IntRect BoundingRectInLocalCoordinates() {return IntRect();}

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid) {}

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {return NULL;}

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX1,
			eNRStyleY1, 
			eNRStyleX2,
			eNRStyleY2,
		};
		return validAttrs;
	}

protected:
	bool OnSelfOrChildStateChange();

	bool m_bChildrenHasUpdata;
	KdPath* m_path;
};

#endif // ClipPathNode_h