#include <UIlib.h>
#include "LinearGradientNode.h"
#include "Dom/Style/NodeStyle.h"
#include "Graphics/KdColor.h"

GradientData* LinearGradientNode::CollectGradientArgByNode(UINode* owner) {
	CollectGradientArgByNodeCommHead();
	
	IntRect ownerRect = owner->BoundingRectInLocalCoordinates();

	float x1 = GetAttrs()->X1().u.percent;
	float y1 = GetAttrs()->Y1().u.percent;
	float x2 = GetAttrs()->X2().u.percent;
	float y2 = GetAttrs()->Y2().u.percent;

	FloatPoint startPoint = FloatPoint(x1, y1);
	FloatPoint endPoint = FloatPoint(x2, y2);

	if (m_gradientData.gradient)
		m_gradientData.gradient = 0;
	m_gradientData.gradient = Gradient::create(startPoint, endPoint);

	return CollectGradientArgByNodeCommEnd(owner);
}