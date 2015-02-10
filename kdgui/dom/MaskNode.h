#ifndef MaskNode_h
#define MaskNode_h

#include <WTF/OwnPtr.h>
#include "UINode.h"
#include "style/NodeAttrDef.h"
#include "graphics/ImageBuffer.h"

enum NodeAttrDef;

class MaskNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	MaskNode();
	virtual ~MaskNode() {}

	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret);

	virtual LPCTSTR GetClass() const {return _SC("mask");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Mask;}

	virtual bool IsResNode() const {return true;}
	virtual BOOL IsContainer() const {return TRUE;}

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid) {}
// 	virtual void OnChildInvalidateAbsoluteRect(const IntRect& rc, bool bForce);
// 	virtual void OnChildInvalidateRelativeRect(const IntRect& rc, bool bForce);
	virtual bool AppendChild(UINode* pControl);
	virtual bool RemoveChild(UINode* pControl);
	virtual void OnDrawResChange(ChildState childState);

	virtual void OnChildSetAttrBegin();
	virtual void OnChildSetAttrEnd();

	virtual IntRect BoundingRectInLocalCoordinates();
	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {return NULL;}

protected:
	void OnSelfOrChildStateChange();
	void CreateImageBufferIfNeeded(UINode* appliedNode);
	void ApplyMask(UINode* appliedNode, GraphicsContext* g, const IntRect& repaintRect);
	void DrawContentIntoMaskImage(UINode* appliedNode);

	OwnPtr<ImageBuffer> m_maskImage;
	IntSize m_imageSize;
	bool m_bChildNeedDraw;
};

#endif // MaskNode_h