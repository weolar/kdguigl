
#include "RectNode.h"
#include "style/NodeStyle.h"
#include "graphics/GraphicsContext.h"
#include "rendering/RenderSVGResource.h"
#include "graphics/KdPath.h"
#include "util/utilhelp.h"
#include "rendering/ClipData.h"
#include "rendering/RenderSupport.h"

RectNode::RectNode() 
	: m_path(NULL)
	, m_bPathHadChange(true) {}

RectNode::~RectNode() {
	UHDeletePtr(&m_path);
}

int RectNode::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	if( 0 == _tcsicmp(func, _T("GetPath"))) {
		*(const KdPath**)arg = ToPath();
		return 0;
	}
	return -1;
}

const KdPath* RectNode::ToPath() {
	if (!m_path)
		m_path = new KdPath();
	else if (!m_bPathHadChange)
		return m_path;

	m_path->clear();

	IntRect rc = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());

	float widthValue = (float)m_nodeStyle->Width();
	if (widthValue <= 0)
		return NULL;

	float heightValue = (float)m_nodeStyle->Height();
	if (heightValue <= 0)
		return NULL;

	float xValue = (float)m_nodeStyle->X();
	float yValue = (float)m_nodeStyle->Y();

	FloatRect rect(xValue, yValue, widthValue, heightValue);

	float rxValue = (float)m_nodeStyle->Rx();
	float ryValue = (float)m_nodeStyle->Ry();
	if (0.0f != rxValue || 0.0f != ryValue) {
		if (0.0f != rxValue && 0.0f == ryValue)
			ryValue = rxValue;
		else if (0.0f == ryValue && 0.0f != ryValue)
			rxValue = ryValue;
		m_path->addRoundedRect(rect, FloatSize(rxValue, ryValue));
		return m_path;
	}

	m_path->addRect(rect);

	return m_path;
}

void RectNode::DrawFillResource(GraphicsContext* context) {
	if (!m_nodeStyle->IsStyleSetted(L"fill"))
		return;
	
	ColorOrUrl fillColor = m_nodeStyle->FillColor();
	if (eCYTColor == fillColor.type) {
		context->setFillColor(fillColor.color|0xff000000, ColorSpaceDeviceRGB);
		context->setFillRule(RULE_NONZERO);
	} else if (eCYTUrl == fillColor.type) {
		UINode* resNode = GetManager()->GetResNodeByUrl(fillColor.url.GetString(), this);
		if (!resNode)
			return;

		GradientData* gradientData = NULL;
		if (0 != resNode->Dispatch(L"GetGradientData", 0, NULL, this, &gradientData))
			gradientData = NULL;

		resNode->Release();
		if (!gradientData)
			return;
		context->setAlpha(m_nodeStyle->FillOpacity());
		context->setFillGradient(gradientData->gradient);
		context->setFillRule(RULE_NONZERO);
		//context->setFillRule(svgStyle->fillRule());
	}

	IntRect rc = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());
	if (m_nodeStyle->Rx() != 0 || m_nodeStyle->Ry() != 0)
		context->fillRoundedRect(rc, m_nodeStyle->Rx(), m_nodeStyle->Ry());
	else
		context->drawRect(rc);
}

//边框
void RectNode::DrawStrokeResource(GraphicsContext* context) {
	bool bStrokeColorSetted = m_nodeStyle->IsStyleSetted(L"stroke");
	bool bStrokeWidthSetted = m_nodeStyle->IsStyleSetted(L"stroke-width");

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

	context->setAlpha(m_nodeStyle->StrokeOpacity());

	IntRect rc = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());
	if (m_nodeStyle->Rx() != 0 || m_nodeStyle->Ry() != 0)
		context->strokeRoundedRect(rc, m_nodeStyle->Rx(), m_nodeStyle->Ry(), SkIntToScalar(nStrokeWidth));
	else
		context->strokeRect(rc, SkIntToScalar(nStrokeWidth));
}

void RectNode::Paint(GraphicsContext* g, IntRect &rcInvalid) {
	GraphicsContextStateSaver stateSaver(*g);
	RenderSupport::PrepareToContent(this, g);
	
	// UINode::Paint(g, rcInvalid);

	DrawFillResource(g);
	DrawStrokeResource(g);
}

// 由于可能在clip-path里面，所以属性变化后，需要上报
bool RectNode::SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {
// 	bool b = UINode::SetAttr(pstrName, pstrValue);
// 	if (!b)
// 		return false;

	IntRect saveRect = BoundingRect();
	if (!m_nodeStyle->SetAttr(pstrName, eNRStyleTypeString, 0, 0, pstrValue))
		return false;

	m_bPathHadChange = NodeStyle::IsStyleChangeBounding(pstrName);

	UINode* parent = GetParent();
	bool bIsInClipPath = false;
	if (parent)
		bIsInClipPath = (L"ClipPath" == CStdString(parent->GetClass()));
	
	if (bIsInClipPath) {
		if (parent)
			parent->Dispatch(L"ChildrenPathHasChange", 0, NULL, NULL, NULL);
	} else {
		if (m_bPathHadChange) // 如果属性引起边框改变，则需要刷新本节点和所有父节点树的边框
			SetBoundingDirty();

		if (IsValidAttr(pstrName)) {// 如果是支持的属性就需要刷新界面
			InvalidateRect(saveRect);
			Invalidate();
		}
	}

	
	if (parent)
		parent->Release();
	return true;
}