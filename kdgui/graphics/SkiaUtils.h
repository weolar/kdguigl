#ifndef SkiaUtils_h
#define SkiaUtils_h

#include "skia/include/core/SkScalar.h"

class GraphicsContext;
class SkCanvas;

// Skia has problems when passed infinite, etc floats, filter them to 0.
inline SkScalar WebCoreFloatToSkScalar(float f)
{
	return SkFloatToScalar(isfinite(f) ? f : 0);
}

inline SkScalar WebCoreDoubleToSkScalar(double d)
{
	return SkDoubleToScalar(isfinite(d) ? d : 0);
}

// Determine if a given WebKit point is contained in a path
bool SkPathContainsPoint(SkPath*, const FloatPoint&, SkPath::FillType);

// Computes the smallest rectangle that, which when drawn to the given canvas,
// will cover the same area as the source rectangle. It will clip to the canvas'
// clip, doing the necessary coordinate transforms.
//
// srcRect and destRect can be the same.
void ClipRectToCanvas(const SkCanvas&, const SkRect& srcRect, SkRect* destRect);

#ifdef _MSC_VER
SkColor SkPMColorToColor(SkPMColor pm);
#endif

GraphicsContext* GetScratchContext();
void DestroyScratchContext();

#endif // SkiaUtils_h