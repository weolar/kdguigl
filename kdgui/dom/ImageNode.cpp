#include <UIlib.h>
#include "ImageNode.h"
#include "GNode.h"
#include "style/NodeStyle.h"
#include "Graphics/GraphicsContext.h"
#include "graphics/PlatformContextNanovg.h"
#include "rendering/RenderSupport.h"
#include "Core/UIManager.h"

ImageNode::ImageNode() {

}

ImageNode::~ImageNode() {

}

IntRect ImageNode::BoundingRectInLocalCoordinates() {
	IntRect rc = UINode::BoundingRectInLocalCoordinates();
	return rc;
}

class SVGPreserveAspectRatio {
public:
	enum SVGMeetOrSliceType {
		SVG_MEETORSLICE_UNKNOWN = 0,
		SVG_MEETORSLICE_MEET = 1,
		SVG_MEETORSLICE_SLICE = 2
	};

	enum SVGPreserveAspectRatioType {
		SVG_PRESERVEASPECTRATIO_UNKNOWN = 0,
		SVG_PRESERVEASPECTRATIO_NONE = 1,
		SVG_PRESERVEASPECTRATIO_XMINYMIN = 2,
		SVG_PRESERVEASPECTRATIO_XMIDYMIN = 3,
		SVG_PRESERVEASPECTRATIO_XMAXYMIN = 4,
		SVG_PRESERVEASPECTRATIO_XMINYMID = 5,
		SVG_PRESERVEASPECTRATIO_XMIDYMID = 6,
		SVG_PRESERVEASPECTRATIO_XMAXYMID = 7,
		SVG_PRESERVEASPECTRATIO_XMINYMAX = 8,
		SVG_PRESERVEASPECTRATIO_XMIDYMAX = 9,
		SVG_PRESERVEASPECTRATIO_XMAXYMAX = 10
	};
};

static void TransformRect(FloatRect& destRect, FloatRect& srcRect) {
	FloatSize imageSize = srcRect.size();
	float origDestWidth = destRect.width();
	float origDestHeight = destRect.height();

	SVGPreserveAspectRatio::SVGMeetOrSliceType m_meetOrSlice = SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET;
	SVGPreserveAspectRatio::SVGPreserveAspectRatioType m_align = SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID;
	switch (m_meetOrSlice) {
		case SVGPreserveAspectRatio::SVG_MEETORSLICE_UNKNOWN:
			break;
		case SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET: {
			float widthToHeightMultiplier = srcRect.height() / srcRect.width();
			if (origDestHeight > origDestWidth * widthToHeightMultiplier) {
				destRect.setHeight(origDestWidth * widthToHeightMultiplier);
				switch (m_align) {
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
						destRect.setY(destRect.y() + origDestHeight / 2 - destRect.height() / 2);
						break;
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMAX:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
						destRect.setY(destRect.y() + origDestHeight - destRect.height());
						break;
					default:
						break;
				}
			}
			if (origDestWidth > origDestHeight / widthToHeightMultiplier) {
				destRect.setWidth(origDestHeight / widthToHeightMultiplier);
				switch (m_align) {
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMIN:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
						destRect.setX(destRect.x() + origDestWidth / 2 - destRect.width() / 2);
						break;
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMIN:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
						destRect.setX(destRect.x() + origDestWidth - destRect.width());
						break;
					default:
						break;
				}
			}
			break;
		}
		case SVGPreserveAspectRatio::SVG_MEETORSLICE_SLICE: {
			float widthToHeightMultiplier = srcRect.height() / srcRect.width();
			// if the destination height is less than the height of the image we'll be drawing
			if (origDestHeight < origDestWidth * widthToHeightMultiplier) {
				float destToSrcMultiplier = srcRect.width() / destRect.width();
				srcRect.setHeight(destRect.height() * destToSrcMultiplier);
				switch (m_align) {
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
						srcRect.setY(destRect.y() + imageSize.height() / 2 - srcRect.height() / 2);
						break;
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMAX:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
						srcRect.setY(destRect.y() + imageSize.height() - srcRect.height());
						break;
					default:
						break;
				}
			}
			// if the destination width is less than the width of the image we'll be drawing
			if (origDestWidth < origDestHeight / widthToHeightMultiplier) {
				float destToSrcMultiplier = srcRect.height() / destRect.height();
				srcRect.setWidth(destRect.width() * destToSrcMultiplier);
				switch (m_align) {
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMIN:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
						srcRect.setX(destRect.x() + imageSize.width() / 2 - srcRect.width() / 2);
						break;
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMIN:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
					case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
						srcRect.setX(destRect.x() + imageSize.width() - srcRect.width());
						break;
					default:
						break;
				}
			}
			break;

		}
	}
}

static void DrawNinePatchY(
	GraphicsContext* g,
	int image,
	const FloatRect& srcRect, 
	const FloatRect& destRect,
	float ninePatchX,
	float ninePatchY
	) {
	FloatRect srcPart;
	FloatRect destPart;

	srcPart = srcRect;
	srcPart.setHeight(ninePatchY);
	destPart = srcPart;
	destPart.setWidth(destRect.width());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制上半段

	srcPart = FloatRect(0, ninePatchY, srcRect.width(), 1);
	destPart = FloatRect(0, ninePatchY, destRect.width(), destRect.height() - srcRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制中间

	srcPart = FloatRect(0, ninePatchY + 1, srcRect.width(), srcRect.height() - ninePatchY);
	destPart = FloatRect(0, ninePatchY + destRect.height() - srcRect.height(), destRect.width(), srcRect.height() - ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制下边
}

static void DrawNinePatchX(
	GraphicsContext* g,
	int image,
	const FloatRect& srcRect, 
	const FloatRect& destRect,
	float ninePatchX,
	float ninePatchY
	) {
	FloatRect srcPart;
	FloatRect destPart;

	srcPart = srcRect;
	srcPart.setWidth(ninePatchX);
	destPart = srcPart;
	destPart.setHeight(destRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制左半段

	srcPart = FloatRect(ninePatchX, 0, 1, srcRect.height());
	destPart = FloatRect(ninePatchX, 0, destRect.width() - srcRect.width(), destRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制中间

	srcPart = FloatRect(ninePatchX + 1, 0, srcRect.width() - ninePatchX, srcRect.height());
	destPart = FloatRect(ninePatchX + destRect.width() - srcRect.width(), 0, srcRect.width() - ninePatchX, destRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制右边
}

static void DrawNinePatchXY(
	GraphicsContext* g,
	int image,
	const FloatRect& srcRect, 
	const FloatRect& destRect,
	float ninePatchX,
	float ninePatchY
	) {
	FloatRect srcPart;
	FloatRect destPart;

	srcPart = FloatRect(0, 0, ninePatchX, ninePatchY);
	destPart = srcPart;
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制左上

	srcPart = FloatRect(ninePatchX, 0, 1, ninePatchY);
	destPart = FloatRect(ninePatchX, 0, destRect.width() - srcRect.width(), ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制上中

	srcPart = FloatRect(ninePatchX + 1, 0, srcRect.width() - ninePatchX, ninePatchY);
	destPart = FloatRect(ninePatchX + destRect.width() - srcRect.width(), 0, srcRect.width() - ninePatchX, ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制右上

	//--
	srcPart = FloatRect(0, ninePatchY, ninePatchX, 1);
	destPart = FloatRect(0, ninePatchY, ninePatchX, destRect.height() - srcRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制左中

	srcPart = FloatRect(ninePatchX, ninePatchY, 1, 1);
	destPart = FloatRect(ninePatchX, ninePatchY, destRect.width() - srcRect.width(), destRect.height() - srcRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制中中

	srcPart = FloatRect(ninePatchX + 1, ninePatchY, srcRect.width() - ninePatchX, 1);
	destPart = FloatRect(ninePatchX + destRect.width() - srcRect.width(), ninePatchY,
		srcRect.width() - ninePatchX, destRect.height() - srcRect.height());
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制右中

	//--
	srcPart = FloatRect(0, ninePatchY + 1, ninePatchX, srcRect.height() - ninePatchY);
	destPart = FloatRect(0, ninePatchY + destRect.height() - srcRect.height(), ninePatchX, srcRect.height() - ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制左下

	srcPart = FloatRect(ninePatchX, ninePatchY + 1, 1, srcRect.height() - ninePatchY);
	destPart = FloatRect(ninePatchX, ninePatchY + destRect.height() - srcRect.height(), destRect.width() - srcRect.width(), srcRect.height() - ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制下中

	srcPart = FloatRect(ninePatchX + 1, ninePatchY + 1, srcRect.width() - ninePatchX, srcRect.height() - ninePatchY);
	destPart = FloatRect(ninePatchX + destRect.width() - srcRect.width(), ninePatchY + destRect.height() - srcRect.height(), 
		srcRect.width() - ninePatchX, srcRect.height() - ninePatchY);
	destPart.move(destRect.x(), destRect.y());
	g->drawImage(image, ColorSpaceDeviceRGB, destPart, srcPart, CompositeSourceOver, true); // 绘制右下
}

static void DrawNinePatch(
	GraphicsContext* g,
	int image,
	const FloatRect& srcRect, 
	const FloatRect& destRect,
	float ninePatchX,
	float ninePatchY
	) {

	bool bCanStretchX = destRect.width() > srcRect.width() && -1 != ninePatchY && ninePatchY <= srcRect.height();
	bool bCanStretchY = destRect.height() > srcRect.height() && -1 != ninePatchX && ninePatchX <= srcRect.width();

	// 如果宽和高都超出，则判断是否有ninePatchX、ninePatchY
	FloatRect srcPart;
	FloatRect destPart;

	if (!bCanStretchX && bCanStretchY) { // 拉伸y
		DrawNinePatchY(g, image, srcRect, destRect, ninePatchX, ninePatchY);
	} else if (bCanStretchX && !bCanStretchY) { // 拉伸x
		DrawNinePatchX(g, image, srcRect, destRect, ninePatchX, ninePatchY);
	} else if (bCanStretchX && bCanStretchY) { // 拉伸x\y
		DrawNinePatchXY(g, image, srcRect, destRect, ninePatchX, ninePatchY);
	} else
		g->drawImage(image, ColorSpaceDeviceRGB, destRect, srcRect, CompositeSourceOver, true);
}

void ImageNode::DrawClipFastWithStretch(GraphicsContext* g, int image, FloatRect& clipRect, FloatRect& stretchRect) {
	float xOffset = clipRect.x() - stretchRect.x(); // 剪切域相对放大后的框的位置
	float yOffset = clipRect.y() - stretchRect.y();

	int width = 0;
	int height = 0;
	NVGcontext* canvas = g->platformContext()->canvas();
	nvgImageSize(canvas, image, &width, &height);

	float widthStretch = stretchRect.width() / width; // 放大倍数
	float heightStretch = stretchRect.height() / height;

	FloatRect srcRect(xOffset / widthStretch, yOffset / heightStretch,
		clipRect.width() / widthStretch, clipRect.height() / heightStretch);

	g->drawImage(image, ColorSpaceDeviceRGB, clipRect, srcRect, CompositeSourceOver, true);
}

void ImageNode::DrawClipFast(GraphicsContext* g, int image, const IntRect* imageClipRect) {
	FloatRect clipRect(*imageClipRect);
	FloatRect stretchRect((float)GetAttrs()->X(), (float)GetAttrs()->Y(), (float)GetAttrs()->Width(), (float)GetAttrs()->Height());

	clipRect.intersect(stretchRect);

	NVGcontext* canvas = g->platformContext()->canvas();
	int width = 0;
	int height = 0;
	nvgImageSize(canvas, image, &width, &height);
	if (clipRect.isEmpty() || 0 == width || 0 == height)
		return;

	if (GetAttrs()->IsStyleSetted(eNRStylePreserveAspectRatio))	{
		DrawClipFastWithStretch(g, image, clipRect, stretchRect);
		return;
	}
		
	FloatRect srcRect(clipRect);
	srcRect.setX(clipRect.x() - stretchRect.x());
	srcRect.setY(clipRect.y() - stretchRect.y());

	g->drawImage(image, ColorSpaceDeviceRGB, clipRect, srcRect, CompositeSourceOver, true);
}

void ImageNode::Paint(GraphicsContext* g, const IntRect &rcInvalid) {	
	CStdString srcUrl = m_nodeStyle->Src();
	srcUrl.MakeLower();
	NVGcontext* canvas = g->platformContext()->canvas();
	int image = GetManager()->GetImageResBySrc(canvas, srcUrl, this);
	if (!image) 
		return;

	int width = 0;
	int height = 0;
	nvgImageSize(canvas, image, &width, &height);

	RenderCommBegin();

	bool bPreserveAspectRatioIsDefer = m_nodeStyle->PreserveAspectRatio() == eNRSPreserveAspectRatioDefer; // true表示拉伸
	BoundingRectInLocalCoordinates(); // 先调用本函数，去获取一次m_rcItem，这样能被缓存下来
	// 这里不用Bounding的结果，是因为Bounding会考虑剪切域
	IntRect rc = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());
	FloatRect srcRect(0, 0, SkIntToScalar(width), SkIntToScalar(height));
	FloatRect destRect(rc);

	const GNode* parent = (const GNode*)m_pParent;
	const IntRect* imageClipRect = NULL;
	if (eNE_G == m_pParent->GetClassEnum())
		imageClipRect = parent->GetFastImageClipRect();

	if (m_nodeStyle->IsNinePatchVailable()) {
		DrawNinePatch(g, image, srcRect, destRect, (float)m_nodeStyle->NinePatchX(), (float)m_nodeStyle->NinePatchY());
	} else if (imageClipRect) {
		DrawClipFast(g, image, imageClipRect);
	} else {
		if (!bPreserveAspectRatioIsDefer)
			TransformRect(destRect, srcRect);
		g->drawImage(image, ColorSpaceDeviceRGB, destRect, srcRect, CompositeSourceOver, true);
	}

	RenderCommEnd();

	return;
}