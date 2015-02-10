
#include "RenderSVGResource.h"
#include "RenderSVGResourceSolidColor.h"

RenderSVGResourceSolidColor::RenderSVGResourceSolidColor()
{
}

RenderSVGResourceSolidColor::~RenderSVGResourceSolidColor()
{
}

bool RenderSVGResourceSolidColor::applyResource(UINode* object, GraphicsContext*& context, unsigned short resourceMode)
{
	// We are NOT allowed to ASSERT(object) here, unlike all other resources.
	// RenderSVGResourceSolidColor is the only resource which may be used from HTML, when rendering
	// SVG Fonts for a HTML document. This will be indicated by a null RenderObject pointer.
	ASSERT(context);
	ASSERT(resourceMode != ApplyToDefaultMode);

	ColorSpace colorSpace = ColorSpaceDeviceRGB;

	NodeStyle* attrs = object->GetAttrs();

	if (resourceMode & ApplyToFillMode) {
		context->setAlpha(attrs->FillOpacity() : 1.0f);
		context->setFillColor(m_color, colorSpace);
		context->setFillRule(attrs->FillRule());

		if (resourceMode & ApplyToTextMode)
			context->setTextDrawingMode(TextModeFill);
	} else if (resourceMode & ApplyToStrokeMode) {
		context->setAlpha(attrs->StrokeOpacity());
		context->setStrokeColor(m_color, colorSpace);

// 		if (style)
// 			SVGRenderSupport::applyStrokeStyleToContext(context, style, object);

// 		if (resourceMode & ApplyToTextMode)
// 			context->setTextDrawingMode(TextModeStroke);
	}

	return true;
}

void RenderSVGResourceSolidColor::postApplyResource(UINode*, GraphicsContext*& context, unsigned short resourceMode, const Path* path)
{
	ASSERT(context);
	ASSERT(resourceMode != ApplyToDefaultMode);

	if (path && !(resourceMode & ApplyToTextMode)) {
		if (resourceMode & ApplyToFillMode)
			context->fillPath(*path);
		else if (resourceMode & ApplyToStrokeMode)
			context->strokePath(*path);
	}
}