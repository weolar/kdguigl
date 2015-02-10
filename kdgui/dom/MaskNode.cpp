#include <UIlib.h>
#include "MaskNode.h"
#include "rendering/MaskRenderArgs.h"
#include "rendering/RenderSupport.h"
#include "graphics/ImageBuffer.h"
#include "Core/UIManager.h"

bool MaskNode::AppendChild(UINode* pControl) {
	bool b = UINode::AppendChild(pControl);
	if (b)
		OnSelfOrChildStateChange();
	return b;
}

bool MaskNode::RemoveChild(UINode* pControl) {
	bool b = UINode::RemoveChild(pControl);
	if (b)
		OnSelfOrChildStateChange();
	return b;
}

void MaskNode::OnDrawResChange(ChildState childState) {
	OnSelfOrChildStateChange();
}

MaskNode::MaskNode() {
	m_bChildNeedDraw = false;
}

int MaskNode::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	if (0 == _tcsicmp(_SC("ApplyMask"), func) && arg) {
		MaskRenderArgs* maskArgs = (MaskRenderArgs*)arg;
		ApplyMask(maskArgs->appliedNode, maskArgs->g, maskArgs->rect);
		return 0;
	}
	return -1;
}

IntRect MaskNode::BoundingRectInLocalCoordinates() {
	if (!m_bBoundingDirty)
		return m_rcItem;
	m_bBoundingDirty = false;

	if (!m_bChildNeedDraw && !m_bBoundingDirty)
		return m_rcItem;

	for(size_t it = 0; it < m_children.size(); ++it) {
		UINode* n = static_cast<UINode*>(m_children[it]);
		// 不考虑子节点的坐标变换
		IntRect childBounding = n->BoundingRectInLocalCoordinates();
		m_rcItem.unite(childBounding);
	}

	return m_rcItem;
}

void MaskNode::CreateImageBufferIfNeeded(UINode* appliedNode) {
 	//IntSize imageSize(m_rcItem.width(), m_rcItem.height());
	if (!appliedNode)
		return;

	IntRect bounding = appliedNode->BoundingRectInLocalCoordinates();
	IntSize imageSize(bounding.width(), bounding.height());
	// 每次mask的子节点有变化，都要强制刷新整个image buf，不关是否边框m_imageSize == imageSize。否则以前的图案会留在里面

	if (m_maskImage.get())
		m_maskImage.clear();
	m_imageSize = imageSize;
	m_maskImage = ImageBuffer::create(m_imageSize, ColorSpaceLinearRGB);
}

void MaskNode::OnChildSetAttrBegin() {
	OnSelfOrChildStateChange();
}

void MaskNode::OnChildSetAttrEnd() {
	OnSelfOrChildStateChange();
}

void MaskNode::OnSelfOrChildStateChange() {
	m_bChildNeedDraw = true;
	m_bBoundingDirty = true; // 不需要调用SetBoundingDirty()，因为不需要通知父节点

	WTF::Vector<UINode*>* owners = m_pManager->GetEffectsResOwnerNodes(this);
	if (!owners)
		return;

	// 子节点绘图消息被转发到拥有者节点上
	for (size_t i = 0; i < owners->size(); ++i)
		owners->at(i)->SetNeedLayout();
}

void MaskNode::DrawContentIntoMaskImage(UINode* appliedNode/*, const IntRect& repaintRect*/) {
	DebugBreak();
// 	if (!appliedNode || !m_bChildNeedDraw)
// 		return;
// 	m_bChildNeedDraw = false;
// 
// 	CreateImageBufferIfNeeded(appliedNode);
// 	if (!m_maskImage.get())
// 		return;
// 	
// 	GraphicsContext* maskImageContext = m_maskImage->context();
// 	ASSERT(maskImageContext);
// 
// 	IntRect maskImageRect(IntPoint(), m_maskImage->size());
// 
// 	SkMatrix absoluteTransform;
// 	RenderSupport::CalculateTransformationToOutermostSVGCoordinateSystem(appliedNode, absoluteTransform);
// 
// 	//FloatRect absoluteTargetRect = absoluteTransform.mapRect(appliedNode->BoundingRectInLocalCoordinates());
// 	SkRect skAbsoluteTargetRect;
// 	absoluteTransform.mapRect(&skAbsoluteTargetRect, appliedNode->BoundingRectInLocalCoordinates());
// 	FloatRect absoluteTargetRect(skAbsoluteTargetRect);
// 	FloatRect& clampedAbsoluteTargetRect = absoluteTargetRect; // SVGImageBufferTools::clampedAbsoluteTargetRectForRenderer(appliedNode, absoluteTargetRect);
// 
// 	// The save/restore pair is needed for clipToImageBuffer - it doesn't work without it on non-Cg platforms.
// 	maskImageContext->save();
// 	maskImageContext->translate(-clampedAbsoluteTargetRect.x(), -clampedAbsoluteTargetRect.y());
// 	maskImageContext->concatCTM(absoluteTransform);
// 
// 	IntRect maskPaintRect(-5000, -5000, 10000, 10000);
// 	for(size_t it = 0; it < m_children.size(); ++it) {
// 		UINode* n = static_cast<UINode*>(m_children[it]);
// 		// 暂时不考虑子节点的坐标变换
// 		n->Paint(maskImageContext, maskPaintRect);
// 	}
// 	maskImageContext->restore();
// 
// 	m_maskImage->transformColorSpace(ColorSpaceDeviceRGB, ColorSpaceLinearRGB);
// 
// 	// Create the luminance mask.
// 	
// 	RefPtr<ByteArray> srcPixelArray = m_maskImage->getUnmultipliedImageData(maskImageRect);
// 
// 	unsigned pixelArrayLength = srcPixelArray->length();
// 	for (unsigned pixelOffset = 0; pixelOffset < pixelArrayLength; pixelOffset += 4) {
// 		unsigned char a = srcPixelArray->get(pixelOffset + 3);
// 		if (!a)
// 			continue;
// 		unsigned char r = srcPixelArray->get(pixelOffset);
// 		unsigned char g = srcPixelArray->get(pixelOffset + 1);
// 		unsigned char b = srcPixelArray->get(pixelOffset + 2);
// 
// 		float luma = (r * 0.2125f + g * 0.7154f + b * 0.0721f) * ((float)a / 255.0f);
// 		srcPixelArray->set(pixelOffset + 3, luma);
// 	}
// 
// 	m_maskImage->putUnmultipliedImageData(srcPixelArray.get(), maskImageRect.size(), maskImageRect, IntPoint());
}

void MaskNode::ApplyMask(UINode* appliedNode, GraphicsContext* g, const IntRect& repaintRect) {
	DebugBreak();
// 	DrawContentIntoMaskImage(appliedNode);
// 
// 	if (!m_maskImage.get())
// 		return;
// 	
// 	IntRect maskImageRect(IntPoint(), m_maskImage->size());
// 	IntRect tempRect(repaintRect.x(), repaintRect.y(), maskImageRect.width(), maskImageRect.height());
// 	IntRect interseRect = intersection(repaintRect, tempRect);
// 	g->clipToImageBuffer(m_maskImage.get(), interseRect);
}