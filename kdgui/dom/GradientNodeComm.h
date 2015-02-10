#ifndef GradientNodeComm_h
#define GradientNodeComm_h

#include "UINode.h"
#include "style/NodeAttrDef.h"
#include "rendering/RenderSVGResource.h"

enum NodeAttrDef;

class GradientNodeComm : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	GradientNodeComm();
	virtual ~GradientNodeComm();

	//virtual void SetManager(CPageManager* pManager);
	virtual bool IsResNode() const {return true;}

	virtual BOOL IsContainer() const {return TRUE;}

	virtual GradientData* CollectGradientArgByNode(UINode* owner) = 0;

	virtual bool AppendChild(UINode* pControl);
	virtual bool RemoveChild(UINode* pControl);
	virtual void OnChildSetAttrBegin();
	virtual void OnChildSetAttrEnd();
	virtual bool SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret);

	virtual IntRect BoundingRectInLocalCoordinates() {return IntRect();}

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid)  {}

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {return NULL;}

protected:
	GradientData* CollectGradientArgByNodeCommEnd(UINode* owner);
	void OnSelfOrChildStateChange(ChildState childState);

	bool m_bGradientDataNeedUpdata;
	GradientData m_gradientData;
};

#define CollectGradientArgByNodeCommHead() \
	if (!m_bGradientDataNeedUpdata) {\
		if (m_gradientData.gradient.get())\
			return &m_gradientData;\
		return NULL;\
	}

#endif // GradientNodeComm_h