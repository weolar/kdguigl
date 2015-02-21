#ifndef TextNode_h
#define TextNode_h

#include "UINode.h"
#include "style/NodeAttrDef.h"

struct TextRun;

class TextNode : public UINode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	TextNode();
	virtual ~TextNode();

	virtual LPCTSTR GetClass() const {return _SC("text");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Text;}

	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret);

	virtual IntRect BoundingRectInLocalCoordinates();

	virtual bool SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY,
			eNRStyleWidth,
			eNRStyleHeight,

			eNRStyleTextAnchor,
			eNRStyleTextDecoration,
			eNRStyleFontSize,

			eNRStyleVisualNodeCommAttr,

			eNRStyleEnd
		};
		return validAttrs;
	}

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);

protected:
	bool CreateTextRunByStyle(const NodeStyle* style, TextRun& textRun);
	void DrawWithEllipsis(GraphicsContext* g, IntRect& bounding, TextRun& textRun);
	IntRect GetBoundingByStyle();

	CStdString m_text;
	bool m_bHadSettedTextStyle; // 为了优化，在全部属性设置完毕前，不要调用CreateTextRunByStyle
	int m_descender;
	IntRect m_rcPosWithoutShadow; // 不考虑阴影的原始大小
	bool m_bNeedEllipsis; // 计算过text 宽度后，发现需要裁剪变成省略号
};

#endif // TextNode_h