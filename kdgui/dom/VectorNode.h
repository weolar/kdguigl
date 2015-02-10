#ifndef VectorNode_h
#define VectorNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

class KdPath;

// 矢量元素，包括矩形、圆，等
class VectorNode : public UINode {
public:
	VectorNode();
	virtual ~VectorNode();

	virtual const KdPath* ToPath() = 0;

	void DrawFillResource(GraphicsContext* context);
	void DrawStrokeResource(GraphicsContext* context);

	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret);

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);

	virtual IntRect BoundingRectInLocalCoordinates();

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData);

protected:
	KdPath* m_path;
	bool m_bPathHadChange;
	//FloatRect m_boundingBox;
};

#endif // VectorNode_h