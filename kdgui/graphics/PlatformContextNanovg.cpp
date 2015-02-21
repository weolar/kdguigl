/*
 * Copyright (c) 2008, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include "config.h"
#include <UIlib.h>
#include "PlatformContextNanovg.h"

#include "AffineTransform.h"
#include "GraphicsContext.h"
//#include "ImageBuffer.h"
#include "SkiaUtils.h"

#ifndef _MSC_VER
#include "dom/style/NodeStyle.h"
#endif
#include "TextRun.h"

#include "ext/image_operations.h"
#include "ext/platform_canvas.h"

//#include "SkBitmap.h"
#include "SkColorPriv.h"
#include "skia/include/effects/SkDashPathEffect.h"
#include "SkShader.h"

#include "KdColor.h"

#include <wtf/RefCountedLeakCounter.h>
#include <wtf/MathExtras.h>
#include <vector>

//namespace WebCore {

extern bool isPathSkiaSafe(const SkMatrix& transform, const SkPath& path);

// State -----------------------------------------------------------------------

// Encapsulates the additional painting state information we store for each
// pushed graphics state.
struct PlatformContextNanovg::State {
	WTF_MAKE_FAST_ALLOCATED;
public:
    State();
    State(const State&);
    ~State();

    // Common shader state.
    float m_alpha;
    SkXfermode::Mode m_xferMode;
    bool m_useAntialiasing;
    SkDrawLooper* m_looper;

    // Fill.
    SkColor m_fillColor;
    SkShader* m_fillShader;

    // Stroke.
    StrokeStyle m_strokeStyle;
    SkColor m_strokeColor;
    SkShader* m_strokeShader;
    float m_strokeThickness;
    int m_dashRatio;  // Ratio of the length of a dash to its width.
    float m_miterLimit;
    SkPaint::Cap m_lineCap;
    SkPaint::Join m_lineJoin;
    SkDashPathEffect* m_dash;

    // Text. (See TextModeFill & friends in GraphicsContext.h.)
    TextDrawingModeFlags m_textDrawingMode;

    // Helper function for applying the state's alpha value to the given input
    // color to produce a new output color.
    SkColor applyAlpha(SkColor) const;

    // If non-empty, the current State is clipped to this image.
    //SkBitmap m_imageBufferClip;

    // If m_imageBufferClip is non-empty, this is the region the image is clipped to.
    FloatRect m_clip;

    // This is a list of clipping paths which are currently active, in the
    // order in which they were pushed.
    WTF::Vector<SkPath> m_antiAliasClipPaths;
    InterpolationQuality m_interpolationQuality;

    // If we currently have a canvas (non-antialiased path) clip applied.
    bool m_canvasClipApplied;

    PlatformContextNanovg::State cloneInheritedProperties();
private:
    // Not supported.
    void operator=(const State&);
};

#ifndef NDEBUG
static RefCountedLeakCounter PlatformContextNanovgStateCounter(_SC("PlatformContextNanovg::State"));
#endif

// Note: Keep theses default values in sync with GraphicsContextState.
PlatformContextNanovg::State::State()
    : m_alpha(1)
    , m_xferMode(SkXfermode::kSrcOver_Mode)
    , m_useAntialiasing(true)
    , m_looper(0)
    , m_fillColor(0xFF000000)
    , m_fillShader(0)
    , m_strokeStyle(SolidStroke)
    , m_strokeColor(KdColor::black)
    , m_strokeShader(0)
    , m_strokeThickness(0)
    , m_dashRatio(3)
    , m_miterLimit(4)
    , m_lineCap(SkPaint::kDefault_Cap)
    , m_lineJoin(SkPaint::kDefault_Join)
    , m_dash(0)
    , m_textDrawingMode(TextModeFill)
    , m_interpolationQuality(InterpolationHigh)
    , m_canvasClipApplied(false)
{
#ifndef NDEBUG
	PlatformContextNanovgStateCounter.increment();
#endif
}

PlatformContextNanovg::State::State(const State& other)
    : m_alpha(other.m_alpha)
    , m_xferMode(other.m_xferMode)
    , m_useAntialiasing(other.m_useAntialiasing)
    , m_looper(other.m_looper)
    , m_fillColor(other.m_fillColor)
    , m_fillShader(other.m_fillShader)
    , m_strokeStyle(other.m_strokeStyle)
    , m_strokeColor(other.m_strokeColor)
    , m_strokeShader(other.m_strokeShader)
    , m_strokeThickness(other.m_strokeThickness)
    , m_dashRatio(other.m_dashRatio)
    , m_miterLimit(other.m_miterLimit)
    , m_lineCap(other.m_lineCap)
    , m_lineJoin(other.m_lineJoin)
    , m_dash(other.m_dash)
    , m_textDrawingMode(other.m_textDrawingMode)
    //, m_imageBufferClip(other.m_imageBufferClip)
    , m_clip(other.m_clip)
    , m_antiAliasClipPaths(other.m_antiAliasClipPaths)
    , m_interpolationQuality(other.m_interpolationQuality)
    , m_canvasClipApplied(other.m_canvasClipApplied)
{
#ifndef NDEBUG
	PlatformContextNanovgStateCounter.increment();
#endif
    // Up the ref count of these. SkSafeRef does nothing if its argument is 0.
    SkSafeRef(m_looper);
    SkSafeRef(m_dash);
    SkSafeRef(m_fillShader);
    SkSafeRef(m_strokeShader);
}

PlatformContextNanovg::State::~State()
{
    SkSafeUnref(m_looper);
    SkSafeUnref(m_dash);
    SkSafeUnref(m_fillShader);
    SkSafeUnref(m_strokeShader);

#ifndef NDEBUG
	PlatformContextNanovgStateCounter.decrement();
#endif
}

// Returns a new State with all of this object's inherited properties copied.
PlatformContextNanovg::State PlatformContextNanovg::State::cloneInheritedProperties()
{
    PlatformContextNanovg::State state(*this);

    // Everything is inherited except for the clip paths.
    state.m_antiAliasClipPaths.clear();

    return state;
}

SkColor PlatformContextNanovg::State::applyAlpha(SkColor c) const
{
    int s = (int)roundf(m_alpha * 256);
    if (s >= 256)
        return c;
    if (s < 0)
        return 0;

    int a = SkAlphaMul(SkColorGetA(c), s);
    return (c & 0x00FFFFFF) | (a << 24);
}

// PlatformContextNanovg ---------------------------------------------------------

#ifndef NDEBUG
static RefCountedLeakCounter PlatformContextNanovgCounter(_SC("PlatformContextNanovg"));
#endif
// Danger: canvas can be NULL.
PlatformContextNanovg::PlatformContextNanovg(NVGcontext* canvas)
    : m_canvas(canvas)
    , m_printing(false)
    , m_drawingToImageBuffer(false)
    , m_accelerationMode(NoAcceleration)
    , m_backingStoreState(None)
{
#ifndef NDEBUG
	PlatformContextNanovgCounter.increment();
#endif
    m_stateStack.push_back(new State());
    m_state = m_stateStack.back();
}

PlatformContextNanovg::~PlatformContextNanovg()
{
#ifndef NDEBUG
	PlatformContextNanovgCounter.decrement();
#endif
	for (size_t i = 0; i < m_stateStack.size(); ++i)
		delete m_stateStack[i];
	m_stateStack.clear();
	m_state = 0;
}

void PlatformContextNanovg::setCanvas(NVGcontext* canvas)
{
    m_canvas = canvas;
}

void PlatformContextNanovg::setDrawingToImageBuffer(bool value)
{
    m_drawingToImageBuffer = value;
}

bool PlatformContextNanovg::isDrawingToImageBuffer() const
{
    return m_drawingToImageBuffer;
}

void PlatformContextNanovg::save()
{
    ASSERT(!hasImageResamplingHint());

    m_stateStack.push_back(new State(m_state->cloneInheritedProperties()));
    m_state = m_stateStack.back();

    // The clip image only needs to be applied once. Reset the image so that we
    // don't attempt to clip multiple times.
    //m_state->m_imageBufferClip.reset();

    // Save our native canvas.
	nvgSave(canvas());
}

void PlatformContextNanovg::beginLayerClippedToImage(const FloatRect& rect,
                                                   const ImageBuffer* imageBuffer)
{
	DebugBreak();
//     // Skia doesn't support clipping to an image, so we create a layer. The next
//     // time restore is invoked the layer and |imageBuffer| are combined to
//     // create the resulting image.
//     m_state->m_clip = rect;
//     SkRect bounds = { SkFloatToScalar(rect.x()), SkFloatToScalar(rect.y()),
//                       SkFloatToScalar(rect.maxX()), SkFloatToScalar(rect.maxY()) };
// 
//     canvas()->clipRect(bounds);
//     canvas()->saveLayerAlpha(&bounds, 255,
//                              static_cast<SkCanvas::SaveFlags>(SkCanvas::kHasAlphaLayer_SaveFlag | SkCanvas::kFullColorLayer_SaveFlag));
//     // Copy off the image as |imageBuffer| may be deleted before restore is invoked.
//     const SkBitmap* bitmap = imageBuffer->context()->platformContext()->bitmap();
//     if (!bitmap->pixelRef()) {
//         // The bitmap owns it's pixels. This happens when we've allocated the
//         // pixels in some way and assigned them directly to the bitmap (as
//         // happens when we allocate a DIB). In this case the assignment operator
//         // does not copy the pixels, rather the copied bitmap ends up
//         // referencing the same pixels. As the pixels may not live as long as we
//         // need it to, we copy the image.
//         bitmap->copyTo(&m_state->m_imageBufferClip, SkBitmap::kARGB_8888_Config);
//     } else {
//         // If there is a pixel ref, we can safely use the assignment operator.
//         m_state->m_imageBufferClip = *bitmap;
//     }
}

void PlatformContextNanovg::clipPathAntiAliased(const SkPath& clipPath)
{
	SkRect rc = clipPath.getBounds();
	nvgScissor(canvas(), rc.x(), rc.y(), rc.width(), rc.height());

	m_state->m_antiAliasClipPaths.push_back(clipPath);

//     // If we are currently tracking any anti-alias clip paths, then we already
//     // have a layer in place and don't need to add another.
//     bool haveLayerOutstanding = m_state->m_antiAliasClipPaths.size() > 0;
// 
//     // See comments in applyAntiAliasedClipPaths about how this works.
//     m_state->m_antiAliasClipPaths.push_back(clipPath);
// 
//     if (!haveLayerOutstanding) {
//         SkRect bounds = clipPath.getBounds();
//         canvas()->saveLayerAlpha(&bounds, 255, static_cast<SkCanvas::SaveFlags>(SkCanvas::kHasAlphaLayer_SaveFlag | SkCanvas::kFullColorLayer_SaveFlag | SkCanvas::kClipToLayer_SaveFlag));
//         // Guards state modification during clipped operations.
//         // The state is popped in applyAntiAliasedClipPaths().
//         canvas()->save();
//     }
}

void PlatformContextNanovg::restore()
{
//     if (!m_state->m_imageBufferClip.empty()) {
//         applyClipFromImage(m_state->m_clip, m_state->m_imageBufferClip);
//         canvas()->restore();
//     }

    if (!m_state->m_antiAliasClipPaths.isEmpty())
        applyAntiAliasedClipPaths(m_state->m_antiAliasClipPaths);

    State* lastStack = m_stateStack.back();
	delete lastStack;
	lastStack = NULL;
	m_stateStack.pop_back();
    m_state = m_stateStack.back();

    // Restore our native canvas.
	nvgRestore(canvas());
}

void PlatformContextNanovg::drawRect(SkRect rect)
{
	int fillcolorNotTransparent = m_state->m_fillColor & 0xFF000000;
	setupPaintForFilling(canvas());

	nvgBeginPath(canvas());
	nvgRect(canvas(), rect.x(), rect.y(), rect.width(), rect.height());
	nvgFill(canvas());

//     SkPaint paint;
//     int fillcolorNotTransparent = m_state->m_fillColor & 0xFF000000;
//     if (fillcolorNotTransparent) {
//         setupPaintForFilling(&paint);
//         canvas()->drawRect(rect, paint);
//     }
// 
//     if (m_state->m_strokeStyle != NoStroke
//         && (m_state->m_strokeColor & 0xFF000000)) {
//         // We do a fill of four rects to simulate the stroke of a border.
//         SkColor oldFillColor = m_state->m_fillColor;
// 
//         // setFillColor() will set the shader to NULL, so save a ref to it now.
//         SkShader* oldFillShader = m_state->m_fillShader;
//         SkSafeRef(oldFillShader);
//         setFillColor(m_state->m_strokeColor);
//         paint.reset();
//         setupPaintForFilling(&paint);
//         SkRect topBorder = { rect.fLeft, rect.fTop, rect.fRight, rect.fTop + 1 };
//         canvas()->drawRect(topBorder, paint);
//         SkRect bottomBorder = { rect.fLeft, rect.fBottom - 1, rect.fRight, rect.fBottom };
//         canvas()->drawRect(bottomBorder, paint);
//         SkRect leftBorder = { rect.fLeft, rect.fTop + 1, rect.fLeft + 1, rect.fBottom - 1 };
//         canvas()->drawRect(leftBorder, paint);
//         SkRect rightBorder = { rect.fRight - 1, rect.fTop + 1, rect.fRight, rect.fBottom - 1 };
//         canvas()->drawRect(rightBorder, paint);
//         setFillColor(oldFillColor);
//         setFillShader(oldFillShader);
//         SkSafeUnref(oldFillShader);
//     }
}

void PlatformContextNanovg::setupPaintCommon() const
{
// #if defined(SK_DEBUG)
//     {
//         SkPaint defaultPaint;
//         SkASSERT(*paint == defaultPaint);
//     }
// #endif
// 
//     paint->setAntiAlias(m_state->m_useAntialiasing);
//     paint->setXfermodeMode(m_state->m_xferMode);
//     paint->setLooper(m_state->m_looper);
}

void PlatformContextNanovg::setupPaintForFilling(NVGcontext* canvas) const
{
    setupPaintCommon();
//     paint->setColor(m_state->applyAlpha(m_state->m_fillColor));
//     paint->setShader(m_state->m_fillShader);
	nvgFillColor(canvas, nvgRGBA(SkColorGetR(m_state->m_fillColor), 
		SkColorGetG(m_state->m_fillColor), SkColorGetB(m_state->m_fillColor), SkColorGetA(m_state->m_fillColor)));
}

float PlatformContextNanovg::setupPaintForStroking(NVGcontext* canvas, SkRect* rect, int length) const
{
    setupPaintCommon();
    float width = m_state->m_strokeThickness;

	SkColor color = m_state->applyAlpha(m_state->m_strokeColor);
	nvgStrokeColor(canvas, nvgRGBA(SkColorGetR(color), 
		SkColorGetG(color), SkColorGetB(color), SkColorGetA(color)));
	
	nvgStrokeWidth(canvas, SkFloatToScalar(width));
	nvgLineCap(canvas, m_state->m_lineCap);
	nvgLineJoin(canvas, m_state->m_lineJoin);
	nvgMiterLimit(canvas, SkFloatToScalar(m_state->m_miterLimit));
//     paint->setColor(m_state->applyAlpha(m_state->m_strokeColor));
//     paint->setShader(m_state->m_strokeShader);
//     paint->setStyle(SkPaint::kStroke_Style);
//     paint->setStrokeWidth(SkFloatToScalar(width));
//     paint->setStrokeCap(m_state->m_lineCap);
//     paint->setStrokeJoin(m_state->m_lineJoin);
//     paint->setStrokeMiter(SkFloatToScalar(m_state->m_miterLimit));
// 
//     if (m_state->m_dash)
//         paint->setPathEffect(m_state->m_dash);
//     else {
//         switch (m_state->m_strokeStyle) {
//         case NoStroke:
//         case SolidStroke:
//             break;
//         case DashedStroke:
//             width = m_state->m_dashRatio * width;
//             // Fall through.
//         case DottedStroke:
//             // Truncate the width, since we don't want fuzzy dots or dashes.
//             int dashLength = static_cast<int>(width);
//             // Subtract off the endcaps, since they're rendered separately.
//             int distance = length - 2 * static_cast<int>(m_state->m_strokeThickness);
//             int phase = 1;
//             if (dashLength > 1) {
//                 // Determine how many dashes or dots we should have.
//                 int numDashes = distance / dashLength;
//                 int remainder = distance % dashLength;
//                 // Adjust the phase to center the dashes within the line.
//                 if (numDashes % 2 == 0) {
//                     // Even:  shift right half a dash, minus half the remainder
//                     phase = (dashLength - remainder) / 2;
//                 } else {
//                     // Odd:  shift right a full dash, minus half the remainder
//                     phase = dashLength - remainder / 2;
//                 }
//             }
//             SkScalar dashLengthSk = SkIntToScalar(dashLength);
//             SkScalar intervals[2] = { dashLengthSk, dashLengthSk };
//             paint->setPathEffect(new SkDashPathEffect(intervals, 2, SkIntToScalar(phase)))->unref();
//         }
//     }

    return width;
}

void PlatformContextNanovg::setDrawLooper(SkDrawLooper* dl)
{
    SkRefCnt_SafeAssign(m_state->m_looper, dl);
}

void PlatformContextNanovg::setMiterLimit(float ml)
{
    m_state->m_miterLimit = ml;
}

void PlatformContextNanovg::setAlpha(float alpha)
{
    m_state->m_alpha = alpha;
}

void PlatformContextNanovg::setLineCap(SkPaint::Cap lc)
{
    m_state->m_lineCap = lc;
}

void PlatformContextNanovg::setLineJoin(SkPaint::Join lj)
{
    m_state->m_lineJoin = lj;
}

void PlatformContextNanovg::setXfermodeMode(SkXfermode::Mode pdm)
{
    m_state->m_xferMode = pdm;
}

void PlatformContextNanovg::setFillColor(SkColor color)
{
    m_state->m_fillColor = color;
    setFillShader(0);
}

SkDrawLooper* PlatformContextNanovg::getDrawLooper() const
{
    return m_state->m_looper;
}

StrokeStyle PlatformContextNanovg::getStrokeStyle() const
{
    return m_state->m_strokeStyle;
}

void PlatformContextNanovg::setStrokeStyle(StrokeStyle strokeStyle)
{
    m_state->m_strokeStyle = strokeStyle;
}

void PlatformContextNanovg::setStrokeColor(SkColor strokeColor)
{
    m_state->m_strokeColor = strokeColor;
    setStrokeShader(0);
}

float PlatformContextNanovg::getStrokeThickness() const
{
    return m_state->m_strokeThickness;
}

void PlatformContextNanovg::setStrokeThickness(float thickness)
{
    m_state->m_strokeThickness = thickness;
}

void PlatformContextNanovg::setStrokeShader(SkShader* strokeShader)
{
    if (strokeShader)
        m_state->m_strokeColor = KdColor::black;

    if (strokeShader != m_state->m_strokeShader) {
        SkSafeUnref(m_state->m_strokeShader);
        m_state->m_strokeShader = strokeShader;
        SkSafeRef(m_state->m_strokeShader);
    }
}

TextDrawingModeFlags PlatformContextNanovg::getTextDrawingMode() const
{
    return m_state->m_textDrawingMode;
}

float PlatformContextNanovg::getAlpha() const
{
    return m_state->m_alpha;
}

int PlatformContextNanovg::getNormalizedAlpha() const
{
    int alpha = (int)roundf(m_state->m_alpha * 256);
    if (alpha > 255)
        alpha = 255;
    else if (alpha < 0)
        alpha = 0;
    return alpha;
}

void PlatformContextNanovg::setTextDrawingMode(TextDrawingModeFlags mode)
{
    // TextModeClip is never used, so we assert that it isn't set:
    // https://bugs.webkit.org/show_bug.cgi?id=21898
    ASSERT(!(mode & TextModeClip));
    m_state->m_textDrawingMode = mode;
}

void PlatformContextNanovg::setUseAntialiasing(bool enable)
{
    m_state->m_useAntialiasing = enable;
}

SkColor PlatformContextNanovg::effectiveFillColor() const
{
    return m_state->applyAlpha(m_state->m_fillColor);
}

SkColor PlatformContextNanovg::effectiveStrokeColor() const
{
    return m_state->applyAlpha(m_state->m_strokeColor);
}

void PlatformContextNanovg::canvasClipPath(const SkPath& path)
{
	DebugBreak();
    m_state->m_canvasClipApplied = true;
    //m_canvas->clipPath(path);
}

void PlatformContextNanovg::setFillShader(SkShader* fillShader)
{
    if (fillShader)
        m_state->m_fillColor = KdColor::black;

    if (fillShader != m_state->m_fillShader) {
        SkSafeUnref(m_state->m_fillShader);
        m_state->m_fillShader = fillShader;
        SkSafeRef(m_state->m_fillShader);
    }
}

InterpolationQuality PlatformContextNanovg::interpolationQuality() const
{
    return m_state->m_interpolationQuality;
}

void PlatformContextNanovg::setInterpolationQuality(InterpolationQuality interpolationQuality)
{
    m_state->m_interpolationQuality = interpolationQuality;
}

void PlatformContextNanovg::setDashPathEffect(SkDashPathEffect* dash)
{
    if (dash != m_state->m_dash) {
        SkSafeUnref(m_state->m_dash);
        m_state->m_dash = dash;
    }
}

void PlatformContextNanovg::paintSkPaint(const SkRect& rect,
                                       const SkPaint& paint)
{
	DebugBreak();
    //m_canvas->drawRect(rect, paint);
}

const SkBitmap* PlatformContextNanovg::bitmap() const
{
	DebugBreak();
	return 0;
    //return &m_canvas->getDevice()->accessBitmap(false);
}

bool PlatformContextNanovg::isNativeFontRenderingAllowed()
{
// #if ENABLE(SKIA_TEXT)
//     return false;
// #else
    if (m_accelerationMode == SkiaGPU)
        return false;
    //return skia::SupportsPlatformPaint(m_canvas);
	DebugBreak();
	return true;
// #endif
}

void PlatformContextNanovg::getImageResamplingHint(IntSize* srcSize, FloatSize* dstSize) const
{
    *srcSize = m_imageResamplingHintSrcSize;
    *dstSize = m_imageResamplingHintDstSize;
}

void PlatformContextNanovg::setImageResamplingHint(const IntSize& srcSize, const FloatSize& dstSize)
{
    m_imageResamplingHintSrcSize = srcSize;
    m_imageResamplingHintDstSize = dstSize;
}

void PlatformContextNanovg::clearImageResamplingHint()
{
    m_imageResamplingHintSrcSize = IntSize();
    m_imageResamplingHintDstSize = FloatSize();
}

bool PlatformContextNanovg::hasImageResamplingHint() const
{
    return !m_imageResamplingHintSrcSize.isEmpty() && !m_imageResamplingHintDstSize.isEmpty();
}

void PlatformContextNanovg::applyClipFromImage(const FloatRect& rect, const SkBitmap& imageBuffer)
{
	DebugBreak();
//     // NOTE: this assumes the image mask contains opaque black for the portions that are to be shown, as such we
//     // only look at the alpha when compositing. I'm not 100% sure this is what WebKit expects for image clipping.
//     SkPaint paint;
//     paint.setXfermodeMode(SkXfermode::kDstIn_Mode);
//     m_canvas->drawBitmap(imageBuffer, SkFloatToScalar(rect.x()), SkFloatToScalar(rect.y()), &paint);
}

void PlatformContextNanovg::applyAntiAliasedClipPaths(WTF::Vector<SkPath>& paths)
{
    // Anti-aliased clipping:
    //
    // Skia's clipping is 1-bit only. Consider what would happen if it were 8-bit:
    // We have a square canvas, filled with white and we declare a circular
    // clipping path. Then we fill twice with a black rectangle. The fractional
    // pixels would first get the correct color (white * alpha + black * (1 -
    // alpha)), but the second fill would apply the alpha to the already
    // modified color and the result would be too dark.
    //
    // This, anti-aliased clipping needs to be performed after the drawing has
    // been done. In order to do this, we create a new layer of the canvas in
    // clipPathAntiAliased and store the clipping path. All drawing is done to
    // the layer's bitmap while it's in effect. When WebKit calls restore() to
    // undo the clipping, this function is called.
    //
    // Here, we walk the list of clipping paths backwards and, for each, we
    // clear outside of the clipping path. We only need a single extra layer
    // for any number of clipping paths.
    //
    // When we call restore on the SkCanvas, the layer's bitmap is composed
    // into the layer below and we end up with correct, anti-aliased clipping.

	nvgResetScissor(m_canvas);
	paths.pop_back();

//     m_canvas->restore();
// 
//     SkPaint paint;
//     paint.setXfermodeMode(SkXfermode::kClear_Mode);
//     paint.setAntiAlias(true);
//     paint.setStyle(SkPaint::kFill_Style);
// 
//     for (size_t i = paths.size() - 1; i < paths.size(); --i) {
//         paths[i].toggleInverseFillType();
//         m_canvas->drawPath(paths[i], paint);
//     }
// 
//     m_canvas->restore();
}

bool PlatformContextNanovg::canAccelerate() const
{
    return !m_state->m_fillShader; // Can't accelerate with a fill gradient or pattern.
}

bool PlatformContextNanovg::canvasClipApplied() const
{
    return m_state->m_canvasClipApplied;
}

void PlatformContextNanovg::setSharedGraphicsContext3D(SharedGraphicsContext3D* context, DrawingBuffer* drawingBuffer, const IntSize& size)
{
    m_accelerationMode = NoAcceleration;
}

void PlatformContextNanovg::prepareForSoftwareDraw() const
{
    if (m_accelerationMode == SkiaGPU)
        return;

    if (m_backingStoreState == Hardware) {
        // Depending on the blend mode we need to do one of a few things:

        // * For associative blend modes, we can draw into an initially empty
        // canvas and then composite the results on top of the hardware drawn
        // results before the next hardware draw or swapBuffers().

        // * For non-associative blend modes we have to do a readback and then
        // software draw.  When we re-upload in this mode we have to blow
        // away whatever is in the hardware backing store (do a copy instead
        // of a compositing operation).

        if (m_state->m_xferMode == SkXfermode::kSrcOver_Mode) {
            // Note that we have rendering results in both the hardware and software backing stores.
            m_backingStoreState = Mixed;
        } else {
            readbackHardwareToSoftware();
            // When we switch back to hardware copy the results, don't composite.
            m_backingStoreState = Software;
        }
    } else if (m_backingStoreState == Mixed) {
        if (m_state->m_xferMode != SkXfermode::kSrcOver_Mode) {
            // Have to composite our currently software drawn data...
            uploadSoftwareToHardware(CompositeSourceOver);
            // then do a readback so we can hardware draw stuff.
            readbackHardwareToSoftware();
            m_backingStoreState = Software;
        }
    } else if (m_backingStoreState == None) {
        m_backingStoreState = Software;
    }
}

void PlatformContextNanovg::prepareForHardwareDraw() const
{
    if (!(m_accelerationMode == GPU))
        return;

    if (m_backingStoreState == Software) {
        // Last drawn in software; upload everything we've drawn.
        uploadSoftwareToHardware(CompositeCopy);
    } else if (m_backingStoreState == Mixed) {
        // Stuff in software/hardware, composite the software stuff on top of
        // the hardware stuff.
        uploadSoftwareToHardware(CompositeSourceOver);
    }
    m_backingStoreState = Hardware;
}

void PlatformContextNanovg::syncSoftwareCanvas() const
{
    if (m_accelerationMode == SkiaGPU)
        return;

    if (m_backingStoreState == Hardware)
        readbackHardwareToSoftware();
    else if (m_backingStoreState == Mixed) {
        // Have to composite our currently software drawn data..
        uploadSoftwareToHardware(CompositeSourceOver);
        // then do a readback.
        readbackHardwareToSoftware();
        m_backingStoreState = Software;
    }
    m_backingStoreState = Software;
}

void PlatformContextNanovg::markDirtyRect(const IntRect& rect)
{
    if (m_accelerationMode != GPU)
        return;

    switch (m_backingStoreState) {
    case Software:
    case Mixed:
        m_softwareDirtyRect.unite(rect);
        return;
    case Hardware:
        return;
    default:
        DebugBreak();
    }
}

void PlatformContextNanovg::uploadSoftwareToHardware(CompositeOperator op) const
{
}

void PlatformContextNanovg::readbackHardwareToSoftware() const
{
}

//} // namespace WebCore
