
#ifndef GraphicsContext__h
#define GraphicsContext__h

#include <wtf/RefPtr.h>
#include "KdColor.h"
#include "KdPath.h"
#include "ColorSpace.h"
#include "Gradient.h"

const int cMisspellingLineThickness = 3;
const int cMisspellingLinePatternWidth = 4;
const int cMisspellingLinePatternGapWidth = 1;

class SkCanvas;
class AffineTransform;
class DrawingBuffer;
class Font;
class Generator;
class GraphicsContextPlatformPrivate;
class ImageBuffer;
class IntRect;
class RoundedIntRect;
class KURL;
class SharedGraphicsContext3D;
struct TextRun;

class PlatformContextNanovg;
typedef PlatformContextNanovg PlatformGraphicsContext;

enum TextDrawingMode {
	TextModeInvisible = 0,
	TextModeFill      = 1 << 0,
	TextModeStroke    = 1 << 1,
	TextModeClip      = 1 << 2
};
typedef unsigned TextDrawingModeFlags;

enum StrokeStyle {
	NoStroke,
	SolidStroke,
	DottedStroke,
	DashedStroke
};

enum InterpolationQuality {
	InterpolationDefault,
	InterpolationNone,
	InterpolationLow,
	InterpolationMedium,
	InterpolationHigh
};

struct GraphicsContextState {
	GraphicsContextState()
		: strokeThickness(0)
		, shadowBlur(0)

		, textDrawingMode(TextModeFill)
		, strokeColor(KdColor::black)
		, fillColor(KdColor::black)
		, strokeStyle(SolidStroke)
		, fillRule(RULE_NONZERO)
		, strokeColorSpace(ColorSpaceDeviceRGB)
		, fillColorSpace(ColorSpaceDeviceRGB)
		, shadowColorSpace(ColorSpaceDeviceRGB)
		, compositeOperator(CompositeSourceOver)
		, shouldAntialias(true)
		, shouldSmoothFonts(true)
		, paintingDisabled(false)
		, shadowsIgnoreTransforms(false)

	{
	}

	RefPtr<Gradient> strokeGradient;
	//RefPtr<Pattern> strokePattern;

	RefPtr<Gradient> fillGradient;
	//RefPtr<Pattern> fillPattern;

	FloatSize shadowOffset;

	float strokeThickness;
	float shadowBlur;

	TextDrawingModeFlags textDrawingMode;

	KdColor strokeColor;
	KdColor fillColor;
	KdColor shadowColor;

	StrokeStyle strokeStyle;
	WindRule fillRule;

	ColorSpace strokeColorSpace;
	ColorSpace fillColorSpace;
	ColorSpace shadowColorSpace;

	CompositeOperator compositeOperator;

	bool shouldAntialias : 1;
	bool shouldSmoothFonts : 1;
	bool paintingDisabled : 1;
	bool shadowsIgnoreTransforms : 1;
};

class GraphicsContext {
	WTF_MAKE_FAST_ALLOCATED;
public:
	GraphicsContext(PlatformGraphicsContext*);
	~GraphicsContext();

	//SkCanvas* context() {return m_skCanvas;}

	void beginFrame(int windowWidth, int windowHeight, float devicePixelRatio);
	void endFrame();

	PlatformGraphicsContext* platformContext() const;

	void savePlatformState();
	void restorePlatformState();

	void save();
	void restore();

	void setAlpha(float);

	KdColor fillColor() const;
	ColorSpace fillColorSpace() const;
	void setFillColor(const KdColor&, ColorSpace);
	WindRule fillRule() const;
	void setFillRule(WindRule);

	void setFillGradient(PassRefPtr<Gradient>);
	Gradient* fillGradient() const;

	bool paintingDisabled() const;
	void setPaintingDisabled(bool);


	// These draw methods will do both stroking and filling.
	// FIXME: ...except drawRect(), which fills properly but always strokes
	// using a 1-pixel stroke inset from the rect borders (of the correct
	// stroke color).
	void drawRect(const IntRect&);
	void drawLine(const IntPoint&, const IntPoint&);
	void drawEllipse(const IntRect&);
	void drawConvexPolygon(size_t numPoints, const FloatPoint*, bool shouldAntialias = false);

	void fillPath(const KdPath&);
	void strokePath(const KdPath&);

	void strokeRoundedRect(const FloatRect& rect, int rx, int ry, float lineWidth);
	void strokeRect(const FloatRect&, float lineWidth);

		// Arc drawing (used by border-radius in CSS) just supports stroking at the moment.
	void strokeArc(const IntRect&, int startAngle, int angleSpan);

	void fillRect(const FloatRect&);
	void fillRect(const FloatRect&, const KdColor&, ColorSpace);
	void fillRect(const FloatRect&, Generator&);
	void fillRoundedRect(const IntRect&, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const KdColor&, ColorSpace);
	void fillRoundedRect(const RoundedIntRect&, const KdColor&, ColorSpace);
	void fillRoundedRect(const IntRect&, int rx, int ry);
	void fillRectWithRoundedHole(const IntRect&, const RoundedIntRect& roundedHoleRect, const KdColor&, ColorSpace);
	
	void clearRect(const FloatRect&);

	void translate(const FloatSize& size) { translate(size.width(), size.height()); }
	void translate(float x, float y);

	void concatCTM(const AffineTransform&);
	void setCTM(const AffineTransform&);
	AffineTransform getCTM() const;

	void concatCTM(const SkMatrix&);
	void setCTM(const SkMatrix&);

	float strokeThickness() const;
	void setStrokeThickness(float);
	StrokeStyle strokeStyle() const;
	void setStrokeStyle(StrokeStyle);
	KdColor strokeColor() const;
	ColorSpace strokeColorSpace() const;
	void setStrokeColor(const KdColor&, ColorSpace);

	void setLineCap(KdLineCap);
	void setLineJoin(KdLineJoin);

	void setShouldAntialias(bool);
	bool shouldAntialias() const;

	void clip(const IntRect&);
	void clip(const FloatRect&);
	void clip(const SkRegion& region);

	void setImageInterpolationQuality(InterpolationQuality);
	InterpolationQuality imageInterpolationQuality() const;

	void drawImage(int image, ColorSpace styleColorSpace, const FloatRect& dest, const FloatRect& src, CompositeOperator op, bool useLowQualityScale);

	void drawText(int x, int y, const TextRun& textRun);

	void clipPath(const KdPath&, WindRule);

	void beginTransparencyLayer(float opacity);
	void endTransparencyLayer();

	void clipToImageBuffer(ImageBuffer* buffer, const FloatRect& rect);

	void setShadow(const FloatSize& offset, float blur, const KdColor& color, ColorSpace colorSpace);
	
private:
	void platformInit(PlatformGraphicsContext*);
	void platformDestroy();

	void setPlatformFillColor(const KdColor&, ColorSpace);
	void setPlatformFillGradient(Gradient*);

	void setPlatformStrokeColor(const KdColor&, ColorSpace);
	void setPlatformStrokeStyle(StrokeStyle);
	void setPlatformStrokeThickness(float);

	void setPlatformShadow(const FloatSize&, float blur, const KdColor&, ColorSpace);
	void clearPlatformShadow();

	void setPlatformShouldAntialias(bool);

protected:
	//SkCanvas* m_skCanvas;

	GraphicsContextPlatformPrivate* m_data;
	GraphicsContextState m_state;
	WTF::Vector<GraphicsContextState> m_stack;
	bool m_updatingControlTints;
};

class GraphicsContextStateSaver {
public:
	GraphicsContextStateSaver(GraphicsContext& context, bool saveAndRestore = true)
		: m_context(context)
		, m_saveAndRestore(saveAndRestore)
	{
		if (m_saveAndRestore)
			m_context.save();
	}

	~GraphicsContextStateSaver()
	{
		if (m_saveAndRestore)
			m_context.restore();
	}

	void save()
	{
		ASSERT(!m_saveAndRestore);
		m_context.save();
		m_saveAndRestore = true;
	}

	void restore()
	{
		ASSERT(m_saveAndRestore);
		m_context.restore();
		m_saveAndRestore = false;
	}

private:
	GraphicsContext& m_context;
	bool m_saveAndRestore;
};

#endif // GraphicsContext__h