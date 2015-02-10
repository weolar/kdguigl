#include <UIlib.h>
#include <wtf/utilhelp.h>

#include "VectorNode.h"
#include "style/NodeStyle.h"
#include "graphics/GraphicsContext.h"
#include "rendering/RenderSVGResource.h"
#include "Graphics/KdPath.h"
#include "rendering/ClipAttrData.h"
#include "rendering/RenderSupport.h"
#include "Graphics/StrokeStyleApplier.h"
#include "core/UIManager.h"

class BoundingRectStrokeStyleApplier : public StrokeStyleApplier {
public:
	BoundingRectStrokeStyleApplier(const UINode* object)
		: m_object(object) {
	}

	void strokeStyle(GraphicsContext* gc) {
		RenderSupport::ApplyStrokeStyleToContext(gc, m_object);
	}

private:
	const UINode* m_object;
};

VectorNode::VectorNode() 
	: m_path(NULL)
	, m_bPathHadChange(true) {}

VectorNode::~VectorNode() {
	UHDeletePtr(&m_path);
}

int VectorNode::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	if( 0 == _tcsicmp(func, _SC("GetPath"))) {
		*(const KdPath**)arg = ToPath();
		return 0;
	}

	return -1;
}

void VectorNode::DrawFillResource(GraphicsContext* context) {
	ColorOrUrl fillColor;
	if (!m_nodeStyle->IsStyleSetted(eNRStyleFillColor))
		fillColor = ColorOrUrl(0xFF000000);
	else
		fillColor = m_nodeStyle->FillColor();

	float fillOpacity = m_nodeStyle->FillOpacity();
	if (eCYTColor == fillColor.type) {
		context->setFillColor(SkColorSetA(fillColor.color, (int)(fillOpacity*255)), ColorSpaceDeviceRGB);
		context->setFillRule(RULE_NONZERO);
	} else if (eCYTUrl == fillColor.type) {
		UINode* resNode = GetManager()->GetEffectsResNodeByUrl(fillColor.url.GetString(), this);
		if (!resNode)
			return;

		GradientData* gradientData = NULL;
		if (0 != resNode->Dispatch(_SC("GetGradientData"), 0, NULL, this, &gradientData))
			gradientData = NULL;

		resNode->deref();
		if (!gradientData)
			return;
		context->setAlpha(m_nodeStyle->FillOpacity());
		context->setFillGradient(gradientData->gradient);
		context->setFillRule(RULE_NONZERO);
		//context->setFillRule(svgStyle->fillRule());
	} else if (eCYTNull == fillColor.type)
		return;

	context->fillPath(*m_path);
}

//边框
void VectorNode::DrawStrokeResource(GraphicsContext* context) {
	bool bStrokeColorSetted = m_nodeStyle->IsStyleSetted(eNRStyleStrokeColor);
	bool bStrokeWidthSetted = m_nodeStyle->IsStyleSetted(eNRStyleStrokeWidth);

	if (!bStrokeColorSetted && !bStrokeWidthSetted)
		return;

	context->setFillColor(0xffffffff, ColorSpaceDeviceRGB);

	KdColor crStroke = KdColor(m_nodeStyle->StrokeColor()|0xff000000);
	
	int nStrokeWidth = m_nodeStyle->StrokeWidth();
	context->setStrokeColor(crStroke, ColorSpaceDeviceRGB);
	context->setStrokeThickness(SkIntToScalar(nStrokeWidth));

	context->setLineCap(ButtCap);
	context->setLineJoin(MiterJoin);
	context->setStrokeStyle(SolidStroke);

	if (eNRSShapeRenderingCrispEdges == m_nodeStyle->ShapeRendering())
		context->setShouldAntialias(false);	

	context->setAlpha(m_nodeStyle->StrokeOpacity());

	context->strokePath(*m_path);
}

bool FastCheckVectorValid(const NodeStyle* attr) {
	if (eCYTNull == attr->FillColor().type && !attr->IsStyleSetted(eNRStyleStrokeColor) && 
		!attr->IsStyleSetted(eNRStyleStrokeWidth)) 
		return false;

	return true;
}

void VectorNode::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
	RenderCommBegin();

	if (ToPath() && FastCheckVectorValid(m_nodeStyle)) {
		DrawFillResource(g);
		DrawStrokeResource(g);
	}

	RenderCommEnd();
}

IntRect VectorNode::BoundingRectInLocalCoordinates() {
	if (!m_path || m_path->isEmpty() || m_bBoundingDirty)
		ToPath();
	
	if (m_path->isEmpty()) {
		m_rcItem = IntRect();
		return m_rcItem;
	}

	// Cache _unclipped_ fill bounding box, used for calculations in resources
	m_rcItem = IntRect(m_path->boundingRect());

	bool bStrokeColorSetted = m_nodeStyle->IsStyleSetted(eNRStyleStrokeColor);
	bool bStrokeWidthSetted = m_nodeStyle->IsStyleSetted(eNRStyleStrokeWidth);

	if (bStrokeColorSetted || bStrokeWidthSetted) {
		int nStrokeWidth = m_nodeStyle->StrokeWidth();
#ifdef _MSC_VER
		BoundingRectStrokeStyleApplier strokeStyle(this);
		m_rcItem.unite(IntRect(m_path->strokeBoundingRect(&strokeStyle)));
		// skia的stroke在宽度为2的时候，存在一个像素的误差

		//if (nStrokeWidth < 2)
		m_rcItem.inflate(1);
#else
		m_rcItem.inflate(nStrokeWidth); // 如果是nanovg实现，简化处理
#endif
	}

	RenderSupport::IntersectClipPathWithResources(this, m_rcItem);

	return m_rcItem;
}

PassRefPtr<UINode> VectorNode::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {
	if (!RenderSupport::CanHittest(this))
		return NULL;

	if (!FastCheckVectorValid(m_nodeStyle)) 
		return NULL;
		
	FloatPoint coordinatePoint;
	if( (uFlags & UIFIND_HITTEST) != 0 && !RenderSupport::IsPointInNodeBoundingWithClippingArea(this, *(FloatPoint*)(pData), &coordinatePoint))
		return NULL;

	// 这里不用strokeContains是为了优化速度。正常来说要用
	if (!m_path->contains(coordinatePoint))
		return NULL;

	if (Proc && !Proc(this, pData, pProcData))
		return NULL;
	return this;
}