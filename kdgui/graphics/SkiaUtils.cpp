
#include <UIlib.h>
#include <wtf/MathExtras.h>
#include <wtf/ThreadSpecific.h>

#include "skia/include/core/SkColorPriv.h"
#include "FloatPoint.h"
#include "SkiaUtils.h"
#include "PlatformContextSkia.h"

bool SkPathContainsPoint(SkPath* originalPath, const FloatPoint& point, SkPath::FillType ft)
{
	SkRegion rgn;
	SkRegion clip;

	SkPath::FillType originalFillType = originalPath->getFillType();

	const SkPath* path = originalPath;
	SkPath scaledPath;
	int scale = 1;

	SkRect bounds = originalPath->getBounds();

	// We can immediately return false if the point is outside the bounding
	// rect.  We don't use bounds.contains() here, since it would exclude
	// points on the right and bottom edges of the bounding rect, and we want
	// to include them.
	SkScalar fX = SkFloatToScalar(point.x());
	SkScalar fY = SkFloatToScalar(point.y());
	if (fX < bounds.fLeft || fX > bounds.fRight || fY < bounds.fTop || fY > bounds.fBottom)
		return false;

	originalPath->setFillType(ft);

	// Skia has trouble with coordinates close to the max signed 16-bit values
	// If we have those, we need to scale. 
	//
	// TODO: remove this code once Skia is patched to work properly with large
	// values
	const SkScalar kMaxCoordinate = SkIntToScalar(1<<15);
	SkScalar biggestCoord = std::max(std::max(std::max(bounds.fRight, bounds.fBottom), -bounds.fLeft), -bounds.fTop);

	if (biggestCoord > kMaxCoordinate) {
		scale = SkScalarCeil(SkScalarDiv(biggestCoord, kMaxCoordinate));

		SkMatrix m;
		m.setScale(SkScalarInvert(SkIntToScalar(scale)), SkScalarInvert(SkIntToScalar(scale)));
		originalPath->transform(m, &scaledPath);
		path = &scaledPath;
	}

	int x = static_cast<int>(floorf(point.x() / scale));
	int y = static_cast<int>(floorf(point.y() / scale));
	clip.setRect(x - 1, y - 1, x + 1, y + 1);

	bool contains = rgn.setPath(*path, clip);

	originalPath->setFillType(originalFillType);
	return contains;
}

void IntersectRectAndRegion(const SkRegion& region, const SkRect& srcRect, SkRect* destRect) {
	DebugBreak();
// 	// The cliperator requires an int rect, so we round out.
// 	SkIRect srcRectRounded;
// 	srcRect.roundOut(&srcRectRounded);
// 
// 	// The Cliperator will iterate over a bunch of rects where our transformed
// 	// rect and the clipping region (which may be non-square) overlap.
// 	SkRegion::Cliperator cliperator(region, srcRectRounded);
// 	if (cliperator.done()) {
// 		destRect->setEmpty();
// 		return;
// 	}
// 
// 	// Get the union of all visible rects in the clip that overlap our bitmap.
// 	SkIRect currentVisibleRect = cliperator.rect();
// 	cliperator.next();
// 	while (!cliperator.done()) {
// 		currentVisibleRect.join(cliperator.rect());
// 		cliperator.next();
// 	}
// 
// 	destRect->set(currentVisibleRect);
}

void ClipRectToCanvas(const SkCanvas& canvas, const SkRect& srcRect, SkRect* destRect) {
// 	// Translate into the canvas' coordinate space. This is where the clipping
// 	// region applies.
// 	SkRect transformedSrc;
// 	canvas.getTotalMatrix().mapRect(&transformedSrc, srcRect);
// 
// 	// Do the intersection.
// 	SkRect transformedDest;
// 	IntersectRectAndRegion(canvas.getTotalClip(), transformedSrc, &transformedDest);
// 
// 	// Now transform it back into world space.
// 	SkMatrix inverseTransform;
// 	canvas.getTotalMatrix().invert(&inverseTransform);
// 	inverseTransform.mapRect(destRect, transformedDest);
}

static WTF::ThreadSpecific<GraphicsContext> gScratchContext;

GraphicsContext* GetScratchContext() {
	if (gScratchContext.hasInit())
		return gScratchContext.get();

	DebugBreak();
	return 0;
// 	// We don't bother checking for failure creating the ImageBuffer, since our
// 	// ImageBuffer initializer won't fail.
// 	SkCanvas* canvas = skia::CreateBitmapCanvas(1, 1, false);
// 	PlatformContextSkia* platformContext = new PlatformContextSkia(canvas);
// 	GraphicsContext* context = new GraphicsContext(platformContext);
// 	context->platformContext()->setDrawingToImageBuffer(true);
// 
// 	// Make the background transparent. It would be nice if this wasn't
// 	// required, but the canvas is currently filled with the magic transparency
// 	// color. Can we have another way to manage this?
// 	canvas->drawARGB(0, 0, 0, 0, SkXfermode::kClear_Mode);
// 
// 	gScratchContext.set(context);
// 
// 	return gScratchContext.get();
}

static U8CPU InvScaleByte(U8CPU component, uint32_t scale)
{
	SkASSERT(component == (uint8_t)component);
	return (component * scale + 0x8000) >> 16;
}

SkColor SkPMColorToColor(SkPMColor pm)
{
	if (!pm)
		return 0;
	unsigned a = SkGetPackedA32(pm);
	if (!a) {
		// A zero alpha value when there are non-zero R, G, or B channels is an
		// invalid premultiplied color (since all channels should have been
		// multiplied by 0 if a=0).
		SkASSERT(false); 
		// In production, return 0 to protect against division by zero.
		return 0;
	}

	uint32_t scale = (255 << 16) / a;

	return SkColorSetARGB(a,
		InvScaleByte(SkGetPackedR32(pm), scale),
		InvScaleByte(SkGetPackedG32(pm), scale),
		InvScaleByte(SkGetPackedB32(pm), scale));
}

void DestroyScratchContext() {
	DebugBreak();
// 	if (!gScratchContext.hasInit()) {
// 		gScratchContext.forceDestroy(false, false);
// 		return;
// 	}
// 
// 	// GraphicsContext的析构不会自动销毁下面的数据，所以手动删除一下
// 	PlatformContextSkia* platformContext = gScratchContext.get()->platformContext();
// 	SkCanvas* canvas = platformContext->canvas();
// 	delete canvas;
// 	delete platformContext;
// 
// 	gScratchContext.forceDestroy(true, false);
}