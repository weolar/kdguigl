
#include <wtf/UtilHelp.h>
#include "GraphicsContext.h"
#include "GraphicsContextPlatformPrivate.h"
#include "PlatformContextSkia.h"
#include "SkiaUtils.h"
#include "TextRun.h"

#include "skia/include/effects/SkBlurMaskFilter.h"
#include "skia/include/effects/SkLayerDrawLooper.h"
#include "skia/include/core/SkColorFilter.h"

// "Seatbelt" functions ------------------------------------------------------
//
// These functions check certain graphics primitives for being "safe".
// Skia has historically crashed when sent crazy data. These functions do
// additional checking to prevent crashes.
//
// Ideally, all of these would be fixed in the graphics layer and we would not
// have to do any checking. You can uncomment the ENSURE_VALUE_SAFETY_FOR_SKIA
// flag to check the graphics layer.

// Disabling these checks (20/01/2010), since we think we've fixed all the Skia
// bugs.  Leaving the code in for now, so we can revert easily if necessary.
// #define ENSURE_VALUE_SAFETY_FOR_SKIA

#ifdef ENSURE_VALUE_SAFETY_FOR_SKIA
static bool isCoordinateSkiaSafe(float coord)
{
	// First check for valid floats.
#if defined(_MSC_VER)
	if (!_finite(coord))
#else
	if (!finite(coord))
#endif
		return false;

	// Skia uses 16.16 fixed point and 26.6 fixed point in various places. If
	// the transformed point exceeds 15 bits, we just declare that it's
	// unreasonable to catch both of these cases.
	static const int maxPointMagnitude = 32767;
	if (coord > maxPointMagnitude || coord < -maxPointMagnitude)
		return false;

	return true;
}
#endif

static bool isPointSkiaSafe(const SkMatrix& transform, const SkPoint& pt)
{
#ifdef ENSURE_VALUE_SAFETY_FOR_SKIA
	// Now check for points that will overflow. We check the *transformed*
	// points since this is what will be rasterized.
	SkPoint xPt;
	transform.mapPoints(&xPt, &pt, 1);
	return isCoordinateSkiaSafe(xPt.fX) && isCoordinateSkiaSafe(xPt.fY);
#else
	return true;
#endif
}

static bool isRectSkiaSafe(const SkMatrix& transform, const SkRect& rc)
{
#ifdef ENSURE_VALUE_SAFETY_FOR_SKIA
	SkPoint topleft = {rc.fLeft, rc.fTop};
	SkPoint bottomright = {rc.fRight, rc.fBottom};
	return isPointSkiaSafe(transform, topleft) && isPointSkiaSafe(transform, bottomright);
#else
	return true;
#endif
}

bool isPathSkiaSafe(const SkMatrix& transform, const SkPath& path)
{
#ifdef ENSURE_VALUE_SAFETY_FOR_SKIA
	SkPoint current_points[4];
	SkPath::Iter iter(path, false);
	for (SkPath::Verb verb = iter.next(current_points);
		verb != SkPath::kDone_Verb;
		verb = iter.next(current_points)) {
			switch (verb) {
		case SkPath::kMove_Verb:
			// This move will be duplicated in the next verb, so we can ignore.
			break;
		case SkPath::kLine_Verb:
			// iter.next returns 2 points.
			if (!isPointSkiaSafe(transform, current_points[0])
				|| !isPointSkiaSafe(transform, current_points[1]))
				return false;
			break;
		case SkPath::kQuad_Verb:
			// iter.next returns 3 points.
			if (!isPointSkiaSafe(transform, current_points[0])
				|| !isPointSkiaSafe(transform, current_points[1])
				|| !isPointSkiaSafe(transform, current_points[2]))
				return false;
			break;
		case SkPath::kCubic_Verb:
			// iter.next returns 4 points.
			if (!isPointSkiaSafe(transform, current_points[0])
				|| !isPointSkiaSafe(transform, current_points[1])
				|| !isPointSkiaSafe(transform, current_points[2])
				|| !isPointSkiaSafe(transform, current_points[3]))
				return false;
			break;
		case SkPath::kClose_Verb:
		case SkPath::kDone_Verb:
		default:
			break;
			}
	}
	return true;
#else
	return true;
#endif
}

// Local helper functions ------------------------------------------------------

void addCornerArc(SkPath* path, const SkRect& rect, const IntSize& size, int startAngle)
{
	SkIRect ir;
	int rx = SkMin32(SkScalarRound(rect.width()), size.width());
	int ry = SkMin32(SkScalarRound(rect.height()), size.height());

	ir.set(-rx, -ry, rx, ry);
	switch (startAngle) {
	case 0:
		ir.offset(SkScalarTruncToInt(rect.fRight - ir.fRight), SkScalarTruncToInt(rect.fBottom - ir.fBottom));
		break;
	case 90:
		ir.offset(SkScalarTruncToInt(rect.fLeft - ir.fLeft), SkScalarTruncToInt(rect.fBottom - ir.fBottom));
		break;
	case 180:
		ir.offset(SkScalarTruncToInt(rect.fLeft - ir.fLeft), SkScalarTruncToInt(rect.fTop - ir.fTop));
		break;
	case 270:
		ir.offset(SkScalarTruncToInt(rect.fRight - ir.fRight), SkScalarTruncToInt(rect.fTop - ir.fTop));
		break;
	default:
		ASSERT(0);
	}

	SkRect r;
	r.set(ir);
	path->arcTo(r, SkIntToScalar(startAngle), SkIntToScalar(90), false);
}

// This may be called with a NULL pointer to create a graphics context that has
// no painting.
void GraphicsContext::platformInit(PlatformGraphicsContext* gc)
{
	m_data = new GraphicsContextPlatformPrivate(gc);
	setPaintingDisabled(!gc || !platformContext()->canvas());
}

void GraphicsContext::platformDestroy()
{
	delete m_data;
}

PlatformGraphicsContext* GraphicsContext::platformContext() const
{
	ASSERT(!paintingDisabled());
	return m_data->context();
}

void GraphicsContext::savePlatformState()
{
	if (paintingDisabled())
		return;
	// Save our private State.
	platformContext()->save();
}

void GraphicsContext::restorePlatformState()
{
	if (paintingDisabled())
		return;
	// Restore our private State.
	platformContext()->restore();
}

void GraphicsContext::setPlatformFillColor(const KdColor& color, ColorSpace colorSpace)
{
	if (paintingDisabled())
		return;

	platformContext()->setFillColor(color.rgb());
}

void GraphicsContext::setPlatformFillGradient(Gradient* gradient)
{
	if (paintingDisabled())
		return;

	platformContext()->setFillShader(gradient->platformGradient());
}

void GraphicsContext::setPlatformStrokeColor(const KdColor& strokecolor, ColorSpace colorSpace)
{
	if (paintingDisabled())
		return;

	platformContext()->setStrokeColor(strokecolor.rgb());
}

void GraphicsContext::setPlatformStrokeStyle(StrokeStyle stroke)
{
	if (paintingDisabled())
		return;

	platformContext()->setStrokeStyle(stroke);
}

void GraphicsContext::setPlatformStrokeThickness(float thickness)
{
	if (paintingDisabled())
		return;

	platformContext()->setStrokeThickness(thickness);
}

void GraphicsContext::clip(const FloatRect& rect)
{
	if (paintingDisabled())
		return;

	SkRect r(rect);
	if (!isRectSkiaSafe(getCTM(), r))
		return;

	platformContext()->prepareForSoftwareDraw();
	platformContext()->canvas()->clipRect(r);
}

void GraphicsContext::clip(const IntRect& rect)
{
	if (paintingDisabled())
		return;

	SkRect r(rect);
	if (!isRectSkiaSafe(getCTM(), r))
		return;

	platformContext()->prepareForSoftwareDraw();
	platformContext()->canvas()->clipRect(r);
}

void GraphicsContext::clip(const SkRegion& region)
{
	platformContext()->prepareForSoftwareDraw();
	platformContext()->canvas()->setClipRegion(region);
}

void GraphicsContext::drawRect(const IntRect& rect)
{
	if (paintingDisabled())
		return;

	platformContext()->prepareForSoftwareDraw();

	SkRect r = rect;
	if (!isRectSkiaSafe(getCTM(), r)) {
		// See the fillRect below.
		ClipRectToCanvas(*platformContext()->canvas(), r, &r);
	}

	platformContext()->drawRect(r);
}

void GraphicsContext::strokeRect(const FloatRect& rect, float lineWidth)
{
	if (paintingDisabled())
		return;

	if (!isRectSkiaSafe(getCTM(), rect))
		return;

	platformContext()->prepareForSoftwareDraw();

	SkPaint paint;
	platformContext()->setupPaintForStroking(&paint, 0, 0);
	paint.setStrokeWidth(WebCoreFloatToSkScalar(lineWidth));
	platformContext()->canvas()->drawRect(rect, paint);
}

void GraphicsContext::strokeRoundedRect(const FloatRect& rect, int rx, int ry, float lineWidth)
{
	if (paintingDisabled())
		return;

	if (!isRectSkiaSafe(getCTM(), rect))
		return;

	platformContext()->prepareForSoftwareDraw();

	SkPaint paint;
	platformContext()->setupPaintForStroking(&paint, 0, 0);
	paint.setStrokeWidth(WebCoreFloatToSkScalar(lineWidth));
#pragma warning(push)  
#pragma warning(disable : 4244)  
	platformContext()->canvas()->drawRoundRect(rect, SkScalarTruncToInt(rx), SkScalarTruncToInt(ry), paint);
#pragma warning(pop)
}

void GraphicsContext::fillPath(const KdPath& pathToFill)
{
	if (paintingDisabled())
		return;

	SkPath path = *pathToFill.platformPath();
	if (!isPathSkiaSafe(getCTM(), path))
		return;

	platformContext()->prepareForSoftwareDraw();

	const GraphicsContextState& state = m_state;
	path.setFillType(state.fillRule == RULE_EVENODD ?
		SkPath::kEvenOdd_FillType : SkPath::kWinding_FillType);

	SkPaint paint;
	platformContext()->setupPaintForFilling(&paint);
	platformContext()->canvas()->drawPath(path, paint);
}

void GraphicsContext::fillRect(const FloatRect& rect)
{
	if (paintingDisabled())
		return;

	SkRect r = rect;
	if (!isRectSkiaSafe(getCTM(), r)) {
		// See the other version of fillRect below.
		ClipRectToCanvas(*platformContext()->canvas(), r, &r);
	}

	platformContext()->save();

	platformContext()->prepareForSoftwareDraw();

	SkPaint paint;
	platformContext()->setupPaintForFilling(&paint);
	platformContext()->canvas()->drawRect(r, paint);

	platformContext()->restore();
}

void GraphicsContext::fillRoundedRect(const IntRect& rc, int rx, int ry)
{
	if (paintingDisabled())
		return;

	SkRect r = rc;
	if (!isRectSkiaSafe(getCTM(), r)) {
		// See the other version of fillRect below.
		ClipRectToCanvas(*platformContext()->canvas(), r, &r);
	}

	platformContext()->save();

	platformContext()->prepareForSoftwareDraw();

	SkPaint paint;
	platformContext()->setupPaintForFilling(&paint);
	platformContext()->canvas()->drawRoundRect(r, SkIntToScalar(rx), SkIntToScalar(ry), paint);

	platformContext()->restore();
}

void GraphicsContext::fillRect(const FloatRect& rect, const KdColor& color, ColorSpace colorSpace)
{
	if (paintingDisabled())
		return;

	platformContext()->prepareForSoftwareDraw();

	SkRect r = rect;
	if (!isRectSkiaSafe(getCTM(), r)) {
		// Special case when the rectangle overflows fixed point. This is a
		// workaround to fix bug 1212844. When the input rectangle is very
		// large, it can overflow Skia's internal fixed point rect. This
		// should be fixable in Skia (since the output bitmap isn't that
		// large), but until that is fixed, we try to handle it ourselves.
		//
		// We manually clip the rectangle to the current clip rect. This
		// will prevent overflow. The rectangle will be transformed to the
		// canvas' coordinate space before it is converted to fixed point
		// so we are guaranteed not to overflow after doing this.
		ClipRectToCanvas(*platformContext()->canvas(), r, &r);
	}

	SkPaint paint;
	platformContext()->setupPaintCommon(&paint);
	paint.setColor(color.rgb());
	platformContext()->canvas()->drawRect(r, paint);
}

void GraphicsContext::fillRoundedRect(const IntRect& rect,
									  const IntSize& topLeft,
									  const IntSize& topRight,
									  const IntSize& bottomLeft,
									  const IntSize& bottomRight,
									  const KdColor& color,
									  ColorSpace colorSpace)
{
	if (paintingDisabled())
		return;

	platformContext()->prepareForSoftwareDraw();

	SkRect r = rect;
	if (!isRectSkiaSafe(getCTM(), r))
		// See fillRect().
		ClipRectToCanvas(*platformContext()->canvas(), r, &r);

	if (topLeft.width() + topRight.width() > rect.width()
		|| bottomLeft.width() + bottomRight.width() > rect.width()
		|| topLeft.height() + bottomLeft.height() > rect.height()
		|| topRight.height() + bottomRight.height() > rect.height()) {
			// Not all the radii fit, return a rect. This matches the behavior of
			// KdPath::createRoundedRectangle. Without this we attempt to draw a round
			// shadow for a square box.
			fillRect(rect, color, colorSpace);
			return;
	}

	SkPath path;
	addCornerArc(&path, r, topRight, 270);
	addCornerArc(&path, r, bottomRight, 0);
	addCornerArc(&path, r, bottomLeft, 90);
	addCornerArc(&path, r, topLeft, 180);

	SkPaint paint;
	platformContext()->setupPaintForFilling(&paint);
	paint.setColor(color.rgb());
	platformContext()->canvas()->drawPath(path, paint);
}

InterpolationQuality GraphicsContext::imageInterpolationQuality() const
{
	return platformContext()->interpolationQuality();
}

void GraphicsContext::setImageInterpolationQuality(InterpolationQuality q)
{
	platformContext()->setInterpolationQuality(q);
}

void GraphicsContext::drawImage(SkBitmap* image, ColorSpace styleColorSpace, const FloatRect& dest, const FloatRect& src, CompositeOperator op, bool useLowQualityScale)
{
	if (paintingDisabled() || !image)
		return;

	//////////////////////////////////////////////////////////////////////////
	// 	SkBlurMaskFilter::BlurFlags flags = SkBlurMaskFilter::kHighQuality_BlurFlag;
	// 	SkMaskFilter* mf = SkBlurMaskFilter::Create(2, SkBlurMaskFilter::kNormal_BlurStyle, flags);
	// 	SkColorFilter* cf = SkColorFilter::CreateModeFilter(0xFF000000, SkXfermode::kSrcIn_Mode);
	// 	skPaint.setMaskFilter(mf)->unref();
	// 	skPaint.setColorFilter(cf)->unref();
	//////////////////////////////////////////////////////////////////////////

	SkPaint paint;
	InterpolationQuality previousInterpolationQuality = imageInterpolationQuality();
	setImageInterpolationQuality(previousInterpolationQuality);
	platformContext()->canvas()->drawBitmapRectToRect(*image, &(SkRect)src, (SkRect)dest, &paint);
	setImageInterpolationQuality(previousInterpolationQuality);
}

void GraphicsContext::drawText(int x, int y, const TextRun& textRun)
{
	SkPaint skPaint;
	platformContext()->setupPaintCommon(&skPaint);

	UHSetSkPaintByTextRun(skPaint, textRun);

	CStringA strUtf8 = CW2A(textRun.characters.GetString(), CP_UTF8);
	platformContext()->canvas()->drawText((LPCSTR)strUtf8, strUtf8.GetLength(), 
		SkIntToScalar(x), SkIntToScalar(y), skPaint);
}

void GraphicsContext::clipPath(const KdPath& pathToClip, WindRule clipRule)
{
	if (paintingDisabled())
		return;

	SkPath path = *pathToClip.platformPath();
	if (!isPathSkiaSafe(getCTM(), path))
		return;

	path.setFillType(clipRule == RULE_EVENODD ? SkPath::kEvenOdd_FillType : SkPath::kWinding_FillType);
	platformContext()->clipPathAntiAliased(path);
}

void GraphicsContext::strokePath(const KdPath& pathToStroke)
{
	if (paintingDisabled())
		return;

	SkPath path = *pathToStroke.platformPath();
	if (!isPathSkiaSafe(getCTM(), path))
		return;

	platformContext()->prepareForSoftwareDraw();

	SkPaint paint;
	platformContext()->setupPaintForStroking(&paint, 0, 0);
	platformContext()->canvas()->drawPath(path, paint);
}

void GraphicsContext::beginTransparencyLayer(float opacity)
{
	if (paintingDisabled())
		return;

	// We need the "alpha" layer flag here because the base layer is opaque
	// (the surface of the page) but layers on top may have transparent parts.
	// Without explicitly setting the alpha flag, the layer will inherit the
	// opaque setting of the base and some things won't work properly.
	platformContext()->canvas()->saveLayerAlpha(
		0,
		static_cast<unsigned char>(opacity * 255),
		static_cast<SkCanvas::SaveFlags>(SkCanvas::kHasAlphaLayer_SaveFlag |
		SkCanvas::kFullColorLayer_SaveFlag));
}

void GraphicsContext::endTransparencyLayer()
{
	if (paintingDisabled())
		return;
	platformContext()->canvas()->restore();
}

void GraphicsContext::clearPlatformShadow()
{
	if (paintingDisabled())
		return;
	platformContext()->setDrawLooper(0);
}

void GraphicsContext::setPlatformShadow(const FloatSize& size,
										float blurFloat,
										const KdColor& color,
										ColorSpace colorSpace)
{
#if 0
	//////////////////////////////////////////////////////////////////////////
	float width = size.width();
	float height = size.height();
	float blur = blurFloat;

	uint32_t mfFlags = SkBlurMaskFilter::kHighQuality_BlurFlag;
	SkXfermode::Mode colorMode = SkXfermode::kSrc_Mode;

	SkColor c;
	if (color.isValid())
		c = color.rgb();
	else
		c = SkColorSetARGB(0xFF/3, 0, 0, 0);    // "std" apple shadow color.

	// TODO(tc): Should we have a max value for the blur?  CG clamps at 1000.0
	// for perf reasons.
	SkLayerDrawLooper* dl = SkNEW(SkLayerDrawLooper); // weolar
	SkAutoUnref aur(dl);

	// top layer, we just draw unchanged
	//dl->addLayer();

	// lower layer contains our offset, blur, and colorfilter
	SkLayerDrawLooper::LayerInfo info;

	info.fPaintBits |= SkLayerDrawLooper::kMaskFilter_Bit; // our blur
	info.fPaintBits |= SkLayerDrawLooper::kColorFilter_Bit;
	info.fColorMode = colorMode;
	info.fOffset.set(width, height);
	info.fPostTranslate = m_state.shadowsIgnoreTransforms;

	SkMaskFilter* mf = SkBlurMaskFilter::Create(blur / 2, SkBlurMaskFilter::kOuter_BlurStyle, mfFlags);
	SkColorFilter* cf = SkColorFilter::CreateModeFilter(c, SkXfermode::kSrcIn_Mode);

	SkPaint* paint = dl->addLayer(info);
	SkSafeUnref(paint->setMaskFilter(mf));
	SkSafeUnref(paint->setColorFilter(cf));

	// dl is now built, just install it
	platformContext()->setDrawLooper(dl);
	//////////////////////////////////////////////////////////////////////////
#else
	if (paintingDisabled())
		return;

	// Detect when there's no effective shadow and clear the looper.
	if (!size.width() && !size.height() && !blurFloat) {
		platformContext()->setDrawLooper(0);
		return;
	}

	float width = size.width();
	float height = size.height();
	float blur = blurFloat;

	uint32_t mfFlags = SkBlurMaskFilter::kHighQuality_BlurFlag;
	SkXfermode::Mode colorMode = SkXfermode::kSrc_Mode;

	if (m_state.shadowsIgnoreTransforms)  {
		// Currently only the GraphicsContext associated with the
		// CanvasRenderingContext for HTMLCanvasElement have shadows ignore
		// Transforms. So with this flag set, we know this state is associated
		// with a CanvasRenderingContext.
		mfFlags |= SkBlurMaskFilter::kIgnoreTransform_BlurFlag;

		// CSS wants us to ignore the original's alpha, but Canvas wants us to
		// modulate with it. Using shadowsIgnoreTransforms to tell us that we're
		// in a Canvas, we change the colormode to kDst_Mode, so we don't overwrite
		// it with our layer's (default opaque-black) color.
		colorMode = SkXfermode::kDst_Mode;

		// CG uses natural orientation for Y axis, but the HTML5 canvas spec
		// does not.
		// So we now flip the height since it was flipped in
		// CanvasRenderingContext in order to work with CG.
		height = -height;
	}

	SkColor c;
	if (color.isValid())
		c = color.rgb();
	else
		c = SkColorSetARGB(0xFF/3, 0, 0, 0);    // "std" apple shadow color.

	// TODO(tc): Should we have a max value for the blur?  CG clamps at 1000.0
	// for perf reasons.

	SkLayerDrawLooper* dl = SkNEW(SkLayerDrawLooper); // weolar
	SkAutoUnref aur(dl);

	// top layer, we just draw unchanged
	dl->addLayer();

	// lower layer contains our offset, blur, and colorfilter
	SkLayerDrawLooper::LayerInfo info;

	info.fPaintBits |= SkLayerDrawLooper::kMaskFilter_Bit; // our blur
	info.fPaintBits |= SkLayerDrawLooper::kColorFilter_Bit;
	info.fColorMode = colorMode;
	info.fOffset.set(width, height);
	info.fPostTranslate = m_state.shadowsIgnoreTransforms;

	SkMaskFilter* mf = SkBlurMaskFilter::Create(blur / 2, SkBlurMaskFilter::kNormal_BlurStyle, mfFlags);
	//SkMaskFilter* mf = SkBlurMaskFilter::Create(blur / 2, SkBlurMaskFilter::kOuter_BlurStyle, mfFlags);

	SkColorFilter* cf = SkColorFilter::CreateModeFilter(c, SkXfermode::kSrcIn_Mode);

	SkPaint* paint = dl->addLayer(info);
	SkSafeUnref(paint->setMaskFilter(mf));
	SkSafeUnref(paint->setColorFilter(cf));

	// dl is now built, just install it
	platformContext()->setDrawLooper(dl);
#endif
}

void GraphicsContext::translate(float w, float h)
{
	if (paintingDisabled())
		return;

	platformContext()->canvas()->translate(WebCoreFloatToSkScalar(w),
		WebCoreFloatToSkScalar(h));
}

void GraphicsContext::setPlatformShouldAntialias(bool enable)
{
	if (paintingDisabled())
		return;

	platformContext()->setUseAntialiasing(enable);
}