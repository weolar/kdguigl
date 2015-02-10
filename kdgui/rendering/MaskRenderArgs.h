#ifndef MaskRenderArgs_h
#define MaskRenderArgs_h

struct MaskRenderArgs {
	MaskRenderArgs(UINode* appliedNode_, GraphicsContext* g_, const IntRect& rect_) {
		appliedNode = appliedNode_;
		g = g_;
		rect = rect_;
	}
	UINode* appliedNode;
	GraphicsContext* g;
	IntRect rect;
};

#endif // MaskRenderArgs_h
