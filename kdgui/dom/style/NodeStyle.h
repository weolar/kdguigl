#ifndef NodeStyle_h
#define NodeStyle_h

#include <hash_map>
#include <WTF/HashMap.h>
#include "NodeAttrDef.h"
#include "NodeAttrDefHash.h"

#include "LenOrPercent.h"
#include "ColorOrUrl.h"

class SkMatrix;
class ClipAttrData;
class KdPath;
class ShadowData;

enum NodeRareStyleTypeDef {
	eNRStyleTypeNone = 0,
	eNRStyleTypeInt = 1,
	eNRStyleTypeFloat = 1000,
	eNRStyleTypeString = 2000,
	eNRStyleTypeColor = 3000,
};

class NodeStyle;
typedef bool (WINAPI *NRSParseFn)(NodeStyle*, NodeAttrDef, NodeRareStyleTypeDef, int, float, const SQChar*);

struct NRSTDefaultVal {
	NRSTDefaultVal();
	NRSTDefaultVal(int i_);
	NRSTDefaultVal(float f_);
	NRSTDefaultVal(LPCTSTR s_);

	CStdString ToString();

	int i;
	float f;
	LPCTSTR s;
	NodeRareStyleTypeDef type;
};

enum NRSVisibility {
	eNRSVisibilityInherit,
	eNRSVisibilityHidden,
};

enum NRSPointerEvents {
	eNRSPointerEventsInherit,
	eNRSPointerEventsNone,
};

enum NRSTextAnchor {
	eNRSTextAnchorStart,
	eNRSTextAnchorMiddle,
	eNRSTextAnchorEnd,
};

enum NRSTextDecoration {
	eNRSTextDecorationNone,
	eNRSTextDecorationUnderline,
};

enum NRSFontWeight {
	eNRSFontWeightNormal,
	eNRSFontWeightBold,
	eNRSFontWeightBolder,
	eNRSFontWeightLighter,
};

enum NRSOverflow {
	eNRSOverflowVisible,
	eNRSOverflowHidden,
};

enum NRSTextOverflow {
	eNRSTextOverflowVisible,
	eNRSTextOverflowEllipsis,
};

enum NRSPreserveAspectRatio {
	eNRSPreserveAspectRatioNone,
	eNRSPreserveAspectRatioDefer,
};

enum NRSShapeRendering {
	eNRSShapeRenderingOptimizeSpeed,
	eNRSShapeRenderingCrispEdges,
};

// union NRSValEnums { // 所有枚举类型的属性值，方便map里用
// 	NRSVisibility visibility;
// };

extern NRSTDefaultVal defaultValNull; // 复杂类型没有默认值
extern NRSTDefaultVal defaultValZero;
extern NRSTDefaultVal defaultValColor;
extern NRSTDefaultVal defaultValStr;
extern NRSTDefaultVal defaultValOpacity;
extern NRSTDefaultVal defaultValWidth;
extern NRSTDefaultVal defaultValVisibility;
extern NRSTDefaultVal defaultValPointerEvents;
extern NRSTDefaultVal defaultValNone;

struct NodeRareStyleType {
	NodeAttrDef style;
	NodeRareStyleTypeDef type;
	const SQChar* name;
	NRSParseFn parseFunc;
	NRSTDefaultVal defaultVal;
	bool bChangeBounding; // 此属性是否影响边框大小
	UINT hash;
};
extern NodeRareStyleType* gNodeRareStyleType;

// #define NodeRareStyleSet(type, attrs) \
// 	type attrs() {\
// 		stdext::hash_map<NodeAttrDef, type>::iterator iter = m_##type##Attrs.find(eNRStyle##attrs);\
// 		if (iter == m_##type##Attrs.end())\
// 			return 0;\
// 		return iter->second;\
// 	}\
// \
// 	void Set##attrs(type value) {\
// 		m_##type##Attrs[eNRStyle##attrs] = value;\
// 	}

#define NodeRareStyleSet(type, realType, attrs) \
	realType attrs() const {\
		return realType(Get##type##AttrById(eNRStyle##attrs));\
	}
// \
// 	void Set##attrs(realType value) {\
// 		m_##type##Attrs[eNRStyle##attrs] = type(value);\
// 	}

#define NodeRareStyleComplexSet(ComplexType, attrs) \
	ComplexType attrs() const {return Get##ComplexType##AttrById(eNRStyle##attrs);}

class UINode;

// 这里把风格（stroke）和属性（x,y）等合在一起
class NodeStyle {
	WTF_MAKE_FAST_ALLOCATED;
public:
	NodeStyle(UINode* n);
	~NodeStyle();

	int X() const {return m_x;};
	//void SetX(int n) {m_x = n;}

	int Y() const {return m_y;}
	void SetY(int n) {m_y = n;}

	int Width() const {return m_width;}
	void SetWidth(int n) {m_width = n;}

	int Height() const {return m_height;}
	void SetHeight(int n) {m_height = n;}

	static bool IsStyleChangeBounding(const SQChar* strKey);
	static NodeAttrDef FindStyleIdByName(const SQChar* strKey);

	bool SetAttr(const SQChar* name, NodeRareStyleTypeDef type, int intVal, float floatVal, const SQChar* strVal);
	CStdString GetAttr(LPCTSTR pstrName) const;

	static bool ParseColor (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseInt (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseFloat (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseMask (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr);
	static bool ParseShadow (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr);
	static bool ParseTransform (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr);
	static bool ParseClipData (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr);
	static bool ParseId (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int i, float f, const SQChar* strAttr);
	static bool ParseEnum (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseNinePatch (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseFontSize (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseColorOrUri (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseString (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseLenOrPercent (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);
	static bool ParseStyle (NodeStyle*, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar*);

	int GetintAttrById(NodeAttrDef attrId) const;
	float GetfloatAttrById(NodeAttrDef attrId) const;
	CStdString GetCStdStringAttrById(NodeAttrDef attrId) const;
	ColorOrUrl GetColorOrUrlAttrById(NodeAttrDef attrId) const;
	LenOrPercent GetLenOrPercentAttrById(NodeAttrDef attrId) const;

	//bool IsStyleSetted(const CStdString& attrId) const;
	bool IsStyleSetted(NodeAttrDef attrId) const;

	CStdString DumpAllAttrs() const;

#if 0
	int StrokeColor() {
		stdext::hash_map<NodeAttrDef, int>::iterator iter = m_intAttrs.find(eNRStyleStrokeColor);
		if (iter == m_intAttrs.end())
			return 0;
		return iter->second;
	}

	void SetStrokeColor(int value) {
		m_intAttrs[eNRStyleStrokeColor] = value;
	}
#endif
	NodeRareStyleSet(CStdString, CStdString, Id)
	
	NodeRareStyleSet(int, int, Rx)
	NodeRareStyleSet(int, int, Ry)

	NodeRareStyleComplexSet(LenOrPercent, Cx)
	NodeRareStyleComplexSet(LenOrPercent, Cy)
	NodeRareStyleComplexSet(LenOrPercent, R)

	NodeRareStyleSet(float, float, Opacity)

 	NodeRareStyleSet(int, unsigned, StrokeColor)
	NodeRareStyleSet(int, int, StrokeWidth)
	NodeRareStyleSet(float, float, StrokeOpacity)
	NodeRareStyleSet(int, int, StrokeDasharray)

	NodeRareStyleComplexSet(ColorOrUrl, FillColor)
	//ColorOrUrl FillColor() {return GetColorOrUrlAttrById(eNRStyleFillColor);}

	NodeRareStyleSet(float, float, FillOpacity)

	NodeRareStyleSet(int, unsigned, StopColor)
	NodeRareStyleSet(float, float, StopOpacity)

	NodeRareStyleSet(CStdString, CStdString, Src)

	NodeRareStyleSet(int, NRSVisibility, Visibility) // 枚举类型统一用int保存
	NodeRareStyleSet(int, NRSPointerEvents, PointerEvents)
	NodeRareStyleSet(int, NRSOverflow, Overflow)
	NodeRareStyleSet(int, NRSPreserveAspectRatio, PreserveAspectRatio)

	NodeRareStyleSet(int, NRSTextDecoration, TextDecoration)
	NodeRareStyleSet(int, NRSTextOverflow, TextOverflow)
	NodeRareStyleSet(int, NRSFontWeight, FontWeight)	

	NodeRareStyleSet(int, NRSShapeRendering, ShapeRendering)

	NodeRareStyleComplexSet(LenOrPercent, X1)
	NodeRareStyleComplexSet(LenOrPercent, Y1)
	NodeRareStyleComplexSet(LenOrPercent, X2)
	NodeRareStyleComplexSet(LenOrPercent, Y2)
	NodeRareStyleComplexSet(LenOrPercent, Offset)

	NodeRareStyleSet(CStdString, CStdString, Mask)

	NodeRareStyleComplexSet(LenOrPercent, Fx)
	NodeRareStyleComplexSet(LenOrPercent, Fy)

	const SkMatrix* GetTransform();
	const KdPath* GetClipPath();
	//const ClipAttrData* GetClipAttrData();
	const ShadowData* GetShadowData() const;

	int NinePatchX() {return m_ninePatchX;}
	int NinePatchY() {return m_ninePatchY;}
	bool IsNinePatchVailable() {return -1 != m_ninePatchX || -1 != m_ninePatchY;}

//#ifndef NDEBUG
	CStdString m_id;
//#endif

protected:
	void SaveOriginalAttr(const SQChar* name, const SQChar* val);
	
	bool _SetAttr(const CStdString& name, NodeRareStyleTypeDef type, int intVal, float floatVal, const SQChar* strVal, NodeAttrDef* outStyle);
	void FixTextStyle(Vector<NodeAttrDef>** oldAttrDefs, WTF::Vector<NodeAttrDef>** newAttrDefs);
	
	UINode* m_n;

	WTF::HashMap<NodeAttrDef, int>  m_intAttrs;
	WTF::HashMap<NodeAttrDef, float>  m_floatAttrs;
	WTF::HashMap<NodeAttrDef, CStdString> m_CStdStringAttrs;
	WTF::HashMap<NodeAttrDef, ColorOrUrl> m_ColorOrUrlAttrs;
	WTF::HashMap<NodeAttrDef, LenOrPercent> m_LenOrPercentAttrs;

	WTF::HashMap<UINT, std::pair<CStdString, CStdString> > m_allAttrs;

	WTF::Vector<NodeAttrDef>* m_styleAttrDefs; // 保存通过style设置进来的属性，详情见FixTextStyle

	SkMatrix* m_transform;
	ClipAttrData* m_clipAttrData;
	ShadowData* m_shadowData;

	KdPath* m_clipData;

	int m_x;
	int m_y;
	int m_width;
	int m_height;

	int m_ninePatchX;
	int m_ninePatchY;
};

#endif // NodeStyle_h
