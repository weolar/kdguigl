#include <UIlib.h>
#include "VectorNodeImpl.h"
#include "Graphics/KdPath.h"
#include "Dom/style/NodeStyle.h"
#include "svg/SVGPathParserFactory.h"

#define ToPathHead() \
	if (!m_path)\
		m_path = new KdPath();\
	else if (!m_bPathHadChange)\
		return m_path;\
	m_path->clear();\

const KdPath* RectNode::ToPath() {
	ToPathHead();

	IntRect rc = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());

	float widthValue = (float)rc.width();
	if (widthValue <= 0)
		return NULL;

	float heightValue = (float)rc.height();
	if (heightValue <= 0)
		return NULL;

	float xValue = (float)rc.x();
	float yValue = (float)rc.y();

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

//////////////////////////////////////////////////////////////////////////

const KdPath* CircleNode::ToPath() {
	ToPathHead();

	LenOrPercent radiusAttr = m_nodeStyle->R();
	int radius = radiusAttr.ToLen();
	if (radius <= 0)
		return 0;

	float cx = (float)m_nodeStyle->Cx().ToLen();
	float cy = (float)m_nodeStyle->Cy().ToLen();

	m_path->addEllipse(FloatRect(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f));
	return m_path;
}

const KdPath* EllipseNode::ToPath() {
	ToPathHead();

	float radiusX = (float)m_nodeStyle->Rx();
	if (radiusX <= 0)
		return 0;

	float radiusY = (float)m_nodeStyle->Ry();
	if (radiusY <= 0)
		return 0;

	float cx = (float)m_nodeStyle->Cx().ToLen();
	float cy = (float)m_nodeStyle->Cy().ToLen();

	m_path->addEllipse(FloatRect(cx - radiusX, cy - radiusY, radiusX * 2, radiusY * 2));
	return m_path;
}

const KdPath* LineNode::ToPath() {
	ToPathHead();

	float x1 = (float)m_nodeStyle->X1().ToLen();
	float x2 = (float)m_nodeStyle->X2().ToLen();
	float y1 = (float)m_nodeStyle->Y1().ToLen();
	float y2 = (float)m_nodeStyle->Y2().ToLen();

	m_path->moveTo(FloatPoint(x1, y1));
	m_path->addLineTo(FloatPoint(x2, y2));
	return m_path; 
}

const KdPath* PathNode::ToPath() {
	ToPathHead();

	CStdString d = m_nodeStyle->GetCStdStringAttrById(eNRStyleD);
	if (d.IsEmpty())
		return 0;
	
	SVGPathParserFactory parserFactort;
	bool b = parserFactort.buildPathFromString(d, *m_path);
	if (!b)
		return 0;
	return m_path;
}