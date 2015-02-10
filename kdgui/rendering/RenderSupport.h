#ifndef RenderSupport_h
#define RenderSupport_h

#define RenderCommBegin() \
	IntRect repaintRc(rcInvalid);\
	if (!RenderSupport::RenderBegin(this, repaintRc))\
		return;\
\
	GraphicsContextStateSaver stateSaver(*g);\
	RenderSupport::PrepareRenderToContent(this, g, repaintRc);

#define RenderCommEnd()\
	RenderSupport::FinishRenderContent(this, g);

class KdPath;
class ClipData;
class GraphicsContext;
class UINode;

class RenderSupport {
public:
	static bool IsPointInNodeBoundingWithClippingArea(UINode* object, const FloatPoint& point, FloatPoint* pCoordinatePoint);
		
	static UINode* GetMaskNodeFromCurrentNode(UINode* curNode);
	static bool CanRender(const UINode* object);
	static bool CanHittest(const UINode* object);
	static bool RenderBegin(UINode* object, IntRect &rcInvalid);

	static void MapRepaintToLocalCoordinates(UINode* object, IntRect &rcInvalid);

	static bool PrepareRenderToContent(UINode* object, GraphicsContext* g, const IntRect& repaintRect);
	static bool FinishRenderContent(UINode* object, GraphicsContext* g);

	static void ApplyStrokeStyleToContext(GraphicsContext* context, const UINode* object);

	static PassRefPtr<ClipData> GetClipPathFromNode(UINode* object);
	static void IntersectClipPathWithResources(UINode* object, IntRect& rc);

	static void CalculateTransformationToOutermostSVGCoordinateSystem(const UINode* renderer, SkMatrix& absoluteTransform);
};

#endif // RenderSupport_h