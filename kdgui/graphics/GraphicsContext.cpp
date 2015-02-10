#include <UIlib.h>
#include "GraphicsContext.h"
//#include "PlatformContextSkia.h"
#include "PlatformContextNanovg.h"
//#include "ImageBuffer.h"

GraphicsContext::GraphicsContext(PlatformGraphicsContext* platformGraphicsContext)
	: m_updatingControlTints(false)
{
	platformInit(platformGraphicsContext);
}

GraphicsContext::~GraphicsContext()
{
	ASSERT(m_stack.isEmpty());
	platformDestroy();
}

void GraphicsContext::setAlpha(float alpha)
{
	platformContext()->setAlpha(alpha);
}

void GraphicsContext::save()
{
	savePlatformState();
}

void GraphicsContext::restore()
{
	restorePlatformState();
}

WindRule GraphicsContext::fillRule() const
{
	return m_state.fillRule;
}

void GraphicsContext::setFillRule(WindRule fillRule)
{
	m_state.fillRule = fillRule;
}

void GraphicsContext::setFillColor(const KdColor& color, ColorSpace colorSpace)
{
	m_state.fillColor = color;
	m_state.fillColorSpace = colorSpace;
	m_state.fillGradient.clear();
	//m_state.fillPattern.clear();
	setPlatformFillColor(color, colorSpace);
}

KdColor GraphicsContext::fillColor() const
{
	return m_state.fillColor;
}

ColorSpace GraphicsContext::fillColorSpace() const
{
	return m_state.fillColorSpace;
}

void GraphicsContext::setFillGradient(PassRefPtr<Gradient> gradient)
{
	ASSERT(gradient);
	if (!gradient) {
		setFillColor(KdColor::black, ColorSpaceDeviceRGB);
		return;
	}
	m_state.fillGradient = gradient;
	//m_state.fillPattern.clear();
	setPlatformFillGradient(m_state.fillGradient.get());
}

Gradient* GraphicsContext::fillGradient() const
{
	return m_state.fillGradient.get();
}

void GraphicsContext::setPaintingDisabled(bool f)
{
	m_state.paintingDisabled = f;
}

bool GraphicsContext::paintingDisabled() const
{
	return m_state.paintingDisabled;
}

AffineTransform GraphicsContext::getCTM() const
{
	float xform[6];
	nvgCurrentTransform(platformContext()->canvas(), xform);
	return AffineTransform(xform[0], xform[1], xform[2], xform[3], xform[4], xform[5]);
// 	const SkMatrix& m = platformContext()->canvas()->getTotalMatrix();
// 	return AffineTransform(SkScalarToDouble(m.getScaleX()),
// 		SkScalarToDouble(m.getSkewY()),
// 		SkScalarToDouble(m.getSkewX()),
// 		SkScalarToDouble(m.getScaleY()),
// 		SkScalarToDouble(m.getTranslateX()),
// 		SkScalarToDouble(m.getTranslateY()));
}

void GraphicsContext::concatCTM(const AffineTransform& affine)
{
	if (paintingDisabled())
		return;
	//platformContext()->canvas()->concat(affine);
	nvgTransform(platformContext()->canvas(), affine.a(), affine.b(), affine.c(), affine.d(), affine.e(), affine.f());
}

void GraphicsContext::setCTM(const AffineTransform& affine)
{
	if (paintingDisabled())
		return;
	DebugBreak();
	//platformContext()->canvas()->setMatrix(affine);
}

void GraphicsContext::concatCTM(const SkMatrix& affine)
{
	if (paintingDisabled())
		return;
	if (!affine.isIdentity())
		nvgTransform(platformContext()->canvas(), affine.get(SkMatrix::kMScaleX),
		affine.get(SkMatrix::kMSkewY), affine.get(SkMatrix::kMSkewX),
		affine.get(SkMatrix::kMScaleY), affine.get(SkMatrix::kMTransX),
		affine.get(SkMatrix::kMTransY));
		//platformContext()->canvas()->concat(affine);
}

void GraphicsContext::setCTM(const SkMatrix& affine)
{
	if (paintingDisabled())
		return;
	//platformContext()->canvas()->setMatrix(affine);
	DebugBreak();
}

void GraphicsContext::fillRect(const FloatRect& rect, Generator& generator)
{
	if (paintingDisabled())
		return;
	generator.fill(this, rect);
}

float GraphicsContext::strokeThickness() const
{
	return m_state.strokeThickness;
}

StrokeStyle GraphicsContext::strokeStyle() const
{
	return m_state.strokeStyle;
}

void GraphicsContext::setStrokeStyle(StrokeStyle style)
{
	m_state.strokeStyle = style;
	setPlatformStrokeStyle(style);
}

KdColor GraphicsContext::strokeColor() const
{
	return m_state.strokeColor;
}

ColorSpace GraphicsContext::strokeColorSpace() const
{
	return m_state.strokeColorSpace;
}

void GraphicsContext::setStrokeColor(const KdColor& color, ColorSpace colorSpace)
{
	m_state.strokeColor = color;
	m_state.strokeColorSpace = colorSpace;
	m_state.strokeGradient.clear();
	//m_state.strokePattern.clear();
	setPlatformStrokeColor(color, colorSpace);
}

void GraphicsContext::setStrokeThickness(float thickness)
{
	m_state.strokeThickness = thickness;
	setPlatformStrokeThickness(thickness);
}

void GraphicsContext::setLineCap(KdLineCap cap)
{
	if (paintingDisabled())
		return;
	switch (cap) {
	case ButtCap:
		platformContext()->setLineCap(SkPaint::kButt_Cap);
		break;
	case RoundCap:
		platformContext()->setLineCap(SkPaint::kRound_Cap);
		break;
	case SquareCap:
		platformContext()->setLineCap(SkPaint::kSquare_Cap);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void GraphicsContext::setLineJoin(KdLineJoin join)
{
	if (paintingDisabled())
		return;
	switch (join) {
	case MiterJoin:
		platformContext()->setLineJoin(SkPaint::kMiter_Join);
		break;
	case RoundJoin:
		platformContext()->setLineJoin(SkPaint::kRound_Join);
		break;
	case BevelJoin:
		platformContext()->setLineJoin(SkPaint::kBevel_Join);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void GraphicsContext::clipToImageBuffer(ImageBuffer* buffer, const FloatRect& rect)
{
#ifdef _MSC_VER
	if (paintingDisabled())
		return;
	buffer->clip(this, rect);
#endif
}

void GraphicsContext::setShadow(const FloatSize& offset, float blur, const KdColor& color, ColorSpace colorSpace)
{
	m_state.shadowOffset = offset;
	m_state.shadowBlur = blur;
	m_state.shadowColor = color;
	m_state.shadowColorSpace = colorSpace;
	setPlatformShadow(offset, blur, color, colorSpace);
}

void GraphicsContext::setShouldAntialias(bool b)
{
	m_state.shouldAntialias = b;
	setPlatformShouldAntialias(b);
}

bool GraphicsContext::shouldAntialias() const
{
	return m_state.shouldAntialias;
}