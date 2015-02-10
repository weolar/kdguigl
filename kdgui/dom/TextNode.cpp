#include <UIlib.h>
#include <wtf/UtilHelp.h>

#include "Style/NodeStyle.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/TextRun.h"
#include "Rendering/RenderSupport.h"
#include "TextNode.h"

const int s_nAdjustY = 0; // 为了兼容老悬浮窗的位置

TextNode::TextNode() {
	m_bHadSettedTextStyle = false;
	m_bNeedEllipsis = false;
}

TextNode::~TextNode() {

}

int TextNode::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	CStdString sFunc(func);
	if (sFunc == _SC("SetTextOfTextNode")) {
		CStdString sArg(strArg);
		sArg.Replace(_SC("\n"), _SC(""));
		sArg.Replace(_SC("\r"), _SC(""));
		if (m_text == sArg)
			return 0;

		SetAttrBegin();
		SetBoundingDirty();
		m_bRepaintDirty = true;
		m_bHadSettedTextStyle = true;
		m_text = sArg;
		SetAttrEnd(); // 设置文本的流程和设置属性差不多，之所以不用设置属性，是因为会转换大小写
		
		return 0;
	} else if (sFunc == _SC("GetTextOfTextNode")) {
		*(CStdString**)ret = &m_text;
		return 0;
	}
	return -1;
}

bool TextNode::SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {
	SetBoundingDirty();
	bool b = UINode::SetAttr(pstrName, pstrValue);
	if (b && (m_nodeStyle->IsStyleSetted(eNRStyleTextAnchor) || m_nodeStyle->IsStyleSetted(eNRStyleFontSize) ||
		m_nodeStyle->IsStyleSetted(eNRStyleTextDecoration) || m_nodeStyle->IsStyleSetted(eNRStyleFontWeight)))
		m_bHadSettedTextStyle = true;
	return b;
}

IntRect TextNode::GetBoundingByStyle() {
	return IntRect();

// 	int x = m_nodeStyle->X(), y = m_nodeStyle->Y(), width = m_nodeStyle->Width();
// 	TextRun textRun(m_text);
// 	if (textRun.characters.IsEmpty()) // 如果是空的，则至少放入一个字符，防止获取到的高度为零
// 		textRun.characters = _SC("U");
// 
// 	textRun.characters.Replace(_SC(' '), _SC('_')); // 修复skia忽略空格的特性
// 
// 	if (!CreateTextRunByStyle(m_nodeStyle, textRun))
// 		return IntRect();
// 
// 	SkPaint skPaint;
// 	UHSetSkPaintByTextRun(skPaint, textRun);
// 	return UHMeasureTextByTextRun(x, y, width, textRun, &m_descender, &m_bNeedEllipsis);
}

IntRect TextNode::BoundingRectInLocalCoordinates() {
	if (!m_bBoundingDirty)
		return m_rcItem;
	m_bBoundingDirty = false;

	m_rcItem = GetBoundingByStyle();

	m_rcPosWithoutShadow = m_rcItem;
	RenderSupport::IntersectClipPathWithResources(this, m_rcItem);

	return m_rcItem;
}

bool TextNode::CreateTextRunByStyle(const NodeStyle* style, TextRun& textRun) {
// 	if (!m_bHadSettedTextStyle)
// 		return false;
// 	
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleTextAnchor)) {
// 		NRSTextAnchor textAnchor = (NRSTextAnchor)m_nodeStyle->GetintAttrById(eNRStyleTextAnchor);
// 		if (eNRSTextAnchorStart != textAnchor) {
// 			KDASSERT(m_nodeStyle->IsStyleSetted(eNRStyleWidth));
// 			if (!m_nodeStyle->IsStyleSetted(eNRStyleWidth))
// 				m_nodeStyle->DumpAllAttrs();
// 
// 			textRun.align = (SkPaint::Align)textAnchor; // 两个枚举类型的值顺序是一样的
// 		}
// 	}
// 
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleFontSize))
// 		textRun.textSize = SkIntToScalar(m_nodeStyle->GetintAttrById(eNRStyleFontSize));
// 
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleTextDecoration)) {
// 		NRSTextDecoration textDecoration = m_nodeStyle->TextDecoration();
// 		if (eNRSTextDecorationUnderline == textDecoration)
// 			textRun.bUnderline = true;
// 	}
// 
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleFontWeight)) {
// 		NRSFontWeight fontWeight = m_nodeStyle->FontWeight();
// 		if (eNRSFontWeightBold == fontWeight)
// 			textRun.bBold = true;
// 	}
// 
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleFillColor))
// 		textRun.color = 0xff000000 | m_nodeStyle->FillColor().ToColor();
// 
// 	if (m_nodeStyle->IsStyleSetted(eNRStyleTextOverflow)) {
// 		NRSTextOverflow overFlow = m_nodeStyle->TextOverflow();
// 		if (eNRSTextOverflowEllipsis == overFlow)
// 			textRun.bOverflowEllipsis = true;
// 	}
// 
// 	CStdString fontFamily = m_nodeStyle->GetCStdStringAttrById(eNRStyleFontFamily);
// 	if (!fontFamily.IsEmpty())
// 		textRun.familyName = fontFamily.GetString();

	return true;
}

void TextNode::DrawWithEllipsis(GraphicsContext* g, SkPaint& skPaint, IntRect& bounding, TextRun& textRun) {
// 	UHSetSkPaintByTextRun(skPaint, textRun);
// 	for (int i = m_text.GetLength(); i > 0; --i) {
// 		CStdString text = m_text.Left(i - 1);
// 		text += _SC("...");
// 
// 		SkRect overflowBounding;
// #ifdef _MSC_VER
// 		CStringA strUtf8(CW2A(text, CP_UTF8));
// #else
// 		CString strUtf8(text);
// #endif
//		skPaint.measureText(strUtf8.GetString(), strUtf8.GetLength(), &overflowBounding);
// 		if (overflowBounding.width() > bounding.width())
// 			continue;
// 
// 		textRun.align = SkPaint::kLeft_Align; // 绘制的时候以左对齐
// 		textRun.characters = text;
// 		g->setFillColor(KdColor(textRun.color), ColorSpaceDeviceRGB);
// 		g->drawText(bounding.x(), bounding.y(), textRun);
// 		break;
// 	}
}

void TextNode::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
// 	RenderCommBegin();
// 
// 	BoundingRectInLocalCoordinates();
// 	IntRect bounding = m_rcPosWithoutShadow;
// 	bounding.move(0, bounding.height());
// 	bounding.move(0, -(m_descender + s_nAdjustY));
// 
// 	SkPaint skPaint;
// 	TextRun textRun(m_text);
// 	if (!CreateTextRunByStyle(m_nodeStyle, textRun))
// 		return;
// 
// 	if (m_bNeedEllipsis) {
// 		DrawWithEllipsis(g, skPaint, bounding, textRun);
// 	} else {
// 		textRun.align = SkPaint::kLeft_Align; // 绘制的时候以左对齐
// 		g->setFillColor(KdColor(textRun.color), ColorSpaceDeviceRGB);
// 		g->drawText(bounding.x(), bounding.y(), textRun);
// 	}
// 
// 	RenderCommEnd();
}