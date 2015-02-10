#ifndef RenderSVGResource_h
#define RenderSVGResource_h

#include "Graphics/Gradient.h"

enum RenderSVGResourceType {
	MaskerResourceType,
	MarkerResourceType,
	PatternResourceType,
	LinearGradientResourceType,
	RadialGradientResourceType,
	SolidColorResourceType,
	FilterResourceType,
	ClipperResourceType,
	SolidClipperResourceType
};

enum RenderSVGResourceMode {
	ApplyToDefaultMode = 1 << 0, // used for all resources except gradient/pattern
	ApplyToFillMode    = 1 << 1,
	ApplyToStrokeMode  = 1 << 2,
	ApplyToTextMode    = 1 << 3 // used in combination with ApplyTo{Fill|Stroke}Mode
};

struct GradientData {
	RefPtr<Gradient> gradient;
	AffineTransform userspaceTransform;
};

#endif // RenderSVGResource_h