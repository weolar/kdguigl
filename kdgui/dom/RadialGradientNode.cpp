#include <UIlib.h>
#include "RadialGradientNode.h"
#include "Dom/Style/NodeStyle.h"
#include "Graphics/KdColor.h"

GradientData* RadialGradientNode::CollectGradientArgByNode(UINode* owner) {
	CollectGradientArgByNodeCommHead();

	IntRect ownerRect = owner->BoundingRectInLocalCoordinates();
	
	float x1 = ownerRect.x() + (GetAttrs()->Fx().u.percent)*(ownerRect.width());
	float y1 = ownerRect.y() + (GetAttrs()->Fy().u.percent)*(ownerRect.height());
	float x2 = ownerRect.x() + (GetAttrs()->Cx().u.percent)*(ownerRect.width());
	float y2 = ownerRect.y() + (GetAttrs()->Cy().u.percent)*(ownerRect.height());

	FloatPoint focalPoint = FloatPoint(GetAttrs()->Fx().u.percent, GetAttrs()->Fy().u.percent);
	FloatPoint centerPoint = FloatPoint(GetAttrs()->Cx().u.percent, GetAttrs()->Cy().u.percent);
	float radius = GetAttrs()->R().u.percent;

	// Eventually adjust focal points, as described below
	float deltaX = focalPoint.x() - centerPoint.x();
	float deltaY = focalPoint.y() - centerPoint.y();
	float radiusMax = 0.99f * radius;

	// Spec: If (fx, fy) lies outside the circle defined by (cx, cy) and r, set
	// (fx, fy) to the point of intersection of the line through (fx, fy) and the circle.
	// We scale the radius by 0.99 to match the behavior of FireFox.
	if (sqrt(deltaX * deltaX + deltaY * deltaY) > radiusMax) {
		float angle = atan2f(deltaY, deltaX);

		deltaX = cosf(angle) * radiusMax;
		deltaY = sinf(angle) * radiusMax;
		focalPoint = FloatPoint(deltaX + centerPoint.x(), deltaY + centerPoint.y());
	}

	if (m_gradientData.gradient)
		m_gradientData.gradient = 0;
	m_gradientData.gradient = Gradient::create(focalPoint,
		0, // SVG does not support a "focus radius"
		centerPoint,
		radius);

	return CollectGradientArgByNodeCommEnd(owner);
}