
#include <UIlib.h>
#include "RenderSupport.h"
#include "dom/style/NodeStyle.h"
#include "dom/UINode.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/KdPath.h"
#include "Core/UIManager.h"
#include "rendering/ClipAttrData.h"
#include "rendering/ClipData.h"
#include "rendering/MaskRenderArgs.h"
#include "rendering/ShadowData.h"

void RenderSupport::IntersectClipPathWithResources(UINode* object, IntRect& rc) {
	// 先剪切
	RefPtr<ClipData> clipData = RenderSupport::GetClipPathFromNode(object);
	const KdPath* clipPath = clipData->Path();
	if (clipPath && !clipPath->isEmpty()) {
		FloatRect clipBoundingRect;
		clipBoundingRect = clipPath->boundingRect();

		rc.intersect(IntRect(clipBoundingRect));
	}

	// 再蒙层
	UINode* maskNode = GetMaskNodeFromCurrentNode(object);
	if (maskNode) {
		IntRect maskBounding = maskNode->BoundingRectInLocalCoordinates();
		rc.intersect(maskBounding);
		maskNode->deref();
	}

	// 再阴影
	if (const ShadowData* shadow = object->GetAttrs()->GetShadowData())
		shadow->adjustRectForShadow(rc);
}

PassRefPtr<ClipData> RenderSupport::GetClipPathFromNode(UINode* object) {
	NodeStyle* style = (NodeStyle*)object->GetAttrs();
	return adoptRef(new ClipData((KdPath*)style->GetClipPath(), false));
}

// 从当前节点的属性中获取蒙层节点
UINode* RenderSupport::GetMaskNodeFromCurrentNode(UINode* curNode) {
	NodeStyle* style = (NodeStyle*)curNode->GetAttrs();
	CStdString maskUrl = style->Mask();

	if (maskUrl.IsEmpty())
		return NULL;

	UINode* maskNode = curNode->GetManager()->GetEffectsResNodeByUrl(maskUrl, curNode);
	if (!maskNode)
		return NULL;

	if (eNE_Mask == maskNode->GetClassEnum())
		return maskNode;

	maskNode->deref();
	return NULL;
}

bool RenderSupport::CanRender(const UINode* object) {
	const NodeStyle* attrs = object->GetAttrs();
	if (eNRSVisibilityHidden == attrs->Visibility() || 0 == attrs->Opacity())
		return false;
	return true;
}

bool RenderSupport::CanHittest(const UINode* object) {
	const NodeStyle* attrs = object->GetAttrs();
	if (eNRSVisibilityHidden == attrs->Visibility() || eNRSPointerEventsNone == attrs->PointerEvents())
		return false;
	return true;
}

bool RenderSupport::IsPointInNodeBoundingWithClippingArea(UINode* object, const FloatPoint& point, FloatPoint* pCoordinatePoint) {
	IntRect rc = object->BoundingRectInLocalCoordinates();

	SkPoint pt = (SkPoint)point;
	UINode* parent = object;

#if 0
	while (parent) {
		const SkMatrix* curTransform = ((NodeStyle*)parent->GetAttrs())->GetTransform();
		if (!curTransform->isIdentity()) {
			SkMatrix transform;
			if (!curTransform->invert(&transform))
				return false;
			
			transform.mapPoints(&pt, 1);
		}

		parent = parent->GetParentTemp();
	}
#else
	SkMatrix transform;
	transform.reset();

	while (parent) {
		const SkMatrix* curTransform = ((NodeStyle*)parent->GetAttrs())->GetTransform();
		if (!transform.postConcat(*curTransform))
			return false;
		parent = parent->GetParentTemp();
	}

	SkMatrix transformMap;
	transformMap.reset();
	if (!transform.invert(&transformMap))
		return false;
	transformMap.mapPoints(&pt, 1);
#endif
	if (pCoordinatePoint) {
		pCoordinatePoint->setX(pt.x());
		pCoordinatePoint->setY(pt.y());
	}

	if (!rc.contains((int)pt.x(), (int)pt.y()))
		return false;
	
	PassRefPtr<ClipData> clipData = RenderSupport::GetClipPathFromNode(object);
	const KdPath* clipPath = clipData->Path();
	if (!clipPath)
		return true;

	return clipPath->contains(FloatPoint(pt), RULE_NONZERO);
}

bool RenderSupport::RenderBegin(UINode* object, IntRect &rcInvalid) {
	RenderSupport::MapRepaintToLocalCoordinates(object, rcInvalid);
	if (!rcInvalid.intersects(object->BoundingRectInLocalCoordinates()))
		return false;

	return CanRender(object);
}

void RenderSupport::MapRepaintToLocalCoordinates(UINode* object, IntRect &rcInvalid) {
	NodeStyle* style = (NodeStyle*)object->GetAttrs();
	SkRect skRect = (SkRect)(rcInvalid);

	if (!style->GetTransform()->isIdentity()) {
		SkMatrix inverseMatrix;
		if (style->GetTransform()->invert(&inverseMatrix))
			inverseMatrix.mapRect(&skRect);
	}
	
	rcInvalid = IntRect(skRect);
}

bool RenderSupport::PrepareRenderToContent(UINode* object, GraphicsContext* g, const IntRect& repaintRect) {
	NodeStyle* style = (NodeStyle*)object->GetAttrs();
	g->concatCTM(*style->GetTransform());

	float opacity = style->Opacity();
	const ShadowData* shadow = style->GetShadowData();
	FloatRect objectRect = object->BoundingRectInLocalCoordinates();

	if (opacity < 1 || shadow) {
		if (opacity < 1) {
			//g->clip(objectRect);
			g->beginTransparencyLayer(opacity);
		}

// 		if (shadow) {
// 			g->clip(objectRect);
// 			g->setShadow(IntSize(shadow->x(), shadow->y()), (float)shadow->blur(), shadow->color(), ColorSpaceDeviceRGB);
// 			g->beginTransparencyLayer(1);
// 		}
	}
	
	// 设置剪切域
	WindRule clipRule = RULE_NONZERO;
	PassRefPtr<ClipData> clipData = RenderSupport::GetClipPathFromNode(object);
	const KdPath* clipPath = clipData->Path();
	if (clipPath && !clipPath->isEmpty())
		g->clipPath(*clipPath, clipRule);
	
	// 设置蒙层
// 	UINode* maskNode = GetMaskNodeFromCurrentNode(object);
// 	if (maskNode) {
// 		MaskRenderArgs maskRenderArgs(object, g, IntRect(objectRect));
// 		maskNode->Dispatch(L"ApplyMask", 0, NULL, &maskRenderArgs, NULL);
// 		maskNode->deref();
// 	}

	return true;
}

bool RenderSupport::FinishRenderContent(UINode* object, GraphicsContext* g) {
	NodeStyle* style = (NodeStyle*)object->GetAttrs();

	if (style->Opacity() < 1)
		g->endTransparencyLayer();

// 	if (style->GetShadowData())
// 		g->endTransparencyLayer();

	return true;
}

void RenderSupport::ApplyStrokeStyleToContext(GraphicsContext* context, const UINode* object) {
	ASSERT(context);
	ASSERT(object);

	const NodeStyle* style = object->GetAttrs();

	KdColor crStroke = KdColor(style->StrokeColor()|0xff000000);
	int nStrokeWidth = style->StrokeWidth();
	context->setStrokeColor(crStroke, ColorSpaceDeviceRGB);
	context->setStrokeThickness(SkIntToScalar(nStrokeWidth));

	context->setLineCap(ButtCap);
	context->setLineJoin(MiterJoin);

	context->setStrokeStyle(SolidStroke);

// 	context->setStrokeThickness(svgStyle->strokeWidth().value(lengthContext));
// 	context->setLineCap(svgStyle->capStyle());
// 	context->setLineJoin(svgStyle->joinStyle());
// 	if (svgStyle->joinStyle() == MiterJoin)
// 		context->setMiterLimit(svgStyle->strokeMiterLimit());
// 
// 	const Vector<SVGLength>& dashes = svgStyle->strokeDashArray();
// 	if (dashes.isEmpty())
// 		context->setStrokeStyle(SolidStroke);
// 	else {
// 		DashArray dashArray;
// 		const Vector<SVGLength>::const_iterator end = dashes.end();
// 		for (Vector<SVGLength>::const_iterator it = dashes.begin(); it != end; ++it)
// 			dashArray.append((*it).value(lengthContext));
// 
// 		context->setLineDash(dashArray, svgStyle->strokeDashOffset().value(lengthContext));
// 	}
}

void RenderSupport::CalculateTransformationToOutermostSVGCoordinateSystem(const UINode* renderer, SkMatrix& absoluteTransform)
{
	UINode* current = (UINode*)renderer;
	ASSERT(current);

	absoluteTransform.reset();

	while (current) {
		SkMatrix transform = *(((NodeStyle*)current->GetAttrs())->GetTransform());
		absoluteTransform.postConcat(transform);
		current = current->GetParentTemp();
	}
}