#include <UIlib.h>
#include <wtf/UtilHelp.h>

#include "NodeStyle.h"
#include "Dom/UINode.h"
#include "Core/UIManager.h"

//#include "graphics/AffineTransform.h"
#include "skia/include/core/SkMatrix.h"
#include "skia/include/utils/SkCamera.h"
#include "Graphics/KdPath.h"
#include "Rendering/ClipAttrData.h"
#include "Rendering/ShadowData.h"
#include "cache/ResCache.h"

NRSTDefaultVal defaultValNull(_SC(""));
NRSTDefaultVal defaultValZero(0);
NRSTDefaultVal defaultValColor(0xffffff);
NRSTDefaultVal defaultValStr(_SC(""));
NRSTDefaultVal defaultValOpacity(1.0f);
NRSTDefaultVal defaultValWidth(1);
NRSTDefaultVal defaultValInherit(_SC("inherit"));
NRSTDefaultVal defaultValNone(_SC("none"));
NRSTDefaultVal defaultValOptimizeSpeed(_SC("optimizespeed"));

NodeRareStyleType* gNodeRareStyleType;

NRSTDefaultVal::NRSTDefaultVal() {
	i = 0;
	f = 0.0f;
	s = _SC("");
	type = eNRStyleTypeInt;
}

NRSTDefaultVal::NRSTDefaultVal(int i_) {
	i = i_;
	type = eNRStyleTypeInt;
}

NRSTDefaultVal::NRSTDefaultVal(float f_) {
	f = f_;
	type = eNRStyleTypeFloat;
}

NRSTDefaultVal::NRSTDefaultVal(LPCTSTR s_) {
	s = s_;
	type = eNRStyleTypeString;
}

CStdString NRSTDefaultVal::ToString() {
	CStdString result;
	if (eNRStyleTypeInt == type) {
		result.Format(_SC("%d"), i);
	} else if (eNRStyleTypeFloat == type) {
		result.Format(_SC("%f"), f);
	} else if (eNRStyleTypeString == type) {
		result = s;
	}
	return result;
}

NodeStyle::NodeStyle(UINode* n)
	: m_transform(NULL)
	, m_clipAttrData(NULL)
	, m_clipData(NULL)
	, m_shadowData(NULL)
	, m_styleAttrDefs(NULL)
	, m_ninePatchX(-1)
	, m_ninePatchY(-1)
	, m_x(0)
	, m_y(0)
	, m_width(0)
	, m_height(0)
	, m_n(n) {

}

NodeStyle::~NodeStyle() {
	UHDeletePtr(&m_transform);
	UHDeletePtr(&m_clipAttrData);
	UHDeletePtr(&m_clipData);
	UHDeletePtr(&m_shadowData);
	UHDeletePtr(&m_styleAttrDefs);
}

static WTF::HashMap<UINT, NodeAttrDef>* GetStyleTypeMap () {
	static WTF::HashMap<UINT, NodeAttrDef>* attrs = 0; // 经测试，hash_map确实比普通map快很多.....
	if (attrs)
		return attrs;

	// 每个属性都必须写在下面，按照顺序
	static NodeRareStyleType sNodeRareStyleType[] = {
		{eNRStyleId, eNRStyleTypeString, _SC("id"), &NodeStyle::ParseId, defaultValStr, false, 0},
		{eNRStyleStyle, eNRStyleTypeString, _SC("style"), &NodeStyle::ParseStyle, defaultValStr, true, 0},

		{eNRStyleX, eNRStyleTypeInt, _SC("x"), &NodeStyle::ParseInt, defaultValZero, true, 0},
		{eNRStyleY, eNRStyleTypeInt, _SC("y"), &NodeStyle::ParseInt, defaultValZero, true, 0},
		{eNRStyleWidth, eNRStyleTypeInt, _SC("width"), &NodeStyle::ParseInt, defaultValZero, true, 0},
		{eNRStyleHeight, eNRStyleTypeInt, _SC("height"), &NodeStyle::ParseInt, defaultValZero, true, 0},

		{eNRStyleRx, eNRStyleTypeInt, _SC("rx"), &NodeStyle::ParseInt, defaultValZero, true, 0},
		{eNRStyleRy, eNRStyleTypeInt, _SC("ry"), &NodeStyle::ParseInt, defaultValZero, true, 0},

		{eNRStyleOpacity, eNRStyleTypeFloat, _SC("opacity"), &NodeStyle::ParseFloat, defaultValOpacity, false, 0},

		{eNRStyleStrokeColor, eNRStyleTypeColor, _SC("stroke"), &NodeStyle::ParseColor, defaultValColor, false, 0},
		{eNRStyleStrokeWidth, eNRStyleTypeInt, _SC("stroke-width"), &NodeStyle::ParseInt, defaultValWidth, true, 0},
		{eNRStyleStrokeOpacity, eNRStyleTypeFloat, _SC("stroke-opacity"), &NodeStyle::ParseFloat, defaultValOpacity, false, 0},
		{eNRStyleStrokeDasharray, eNRStyleTypeInt, _SC("stroke-dasharray"), 0, NULL, false, 0},
		{eNRStyleFillColor, eNRStyleTypeColor, _SC("fill"), &NodeStyle::ParseColorOrUri, defaultValColor, false, 0},
		{eNRStyleFillOpacity, eNRStyleTypeFloat, _SC("fill-opacity"), &NodeStyle::ParseFloat, defaultValOpacity, false, 0},

		{eNRStyleStopColor, eNRStyleTypeColor, _SC("stop-color"), &NodeStyle::ParseColor, defaultValColor, false, 0},
		{eNRStyleStopOpacity, eNRStyleTypeFloat, _SC("stop-opacity"), &NodeStyle::ParseFloat, defaultValOpacity, false, 0},

		{eNRStyleSrc, eNRStyleTypeString, _SC("src"), &NodeStyle::ParseString, defaultValStr, false, 0},

		{eNRStyleVisibility, eNRStyleTypeString, _SC("visibility"), &NodeStyle::ParseEnum, defaultValInherit, true, 0},
		{eNRStylePointerEvents, eNRStyleTypeString, _SC("pointer-events"), &NodeStyle::ParseEnum, defaultValInherit, false, 0},

		{eNRStyleX1, eNRStyleTypeNone, _SC("x1"), &NodeStyle::ParseLenOrPercent, defaultValNull, true, 0},
		{eNRStyleY1, eNRStyleTypeNone, _SC("y1"), &NodeStyle::ParseLenOrPercent, defaultValNull, true, 0},
		{eNRStyleX2, eNRStyleTypeNone, _SC("x2"), &NodeStyle::ParseLenOrPercent, defaultValNull, true, 0},
		{eNRStyleY2, eNRStyleTypeNone, _SC("y2"), &NodeStyle::ParseLenOrPercent, defaultValNull, true, 0},
		{eNRStyleOffset, eNRStyleTypeNone, _SC("offset"), &NodeStyle::ParseLenOrPercent, defaultValNull, false, 0},

		{eNRStyleTextAnchor, eNRStyleTypeString, _SC("text-anchor"), &NodeStyle::ParseEnum, NULL, true, 0},
		{eNRStyleTextDecoration, eNRStyleTypeString, _SC("text-decoration"), &NodeStyle::ParseEnum, NULL, false},
		{eNRStyleTextOverflow, eNRStyleTypeString, _SC("text-overflow"), &NodeStyle::ParseEnum, NULL, false},
		
		{eNRStyleFontSize, eNRStyleTypeString, _SC("font-size"), &NodeStyle::ParseFontSize, NULL, true, 0},
		{eNRStyleFontFamily, eNRStyleTypeString, _SC("font-family"), &NodeStyle::ParseString, NULL, true, 0},
		{eNRStyleFontWeight, eNRStyleTypeString, _SC("font-weight"), &NodeStyle::ParseEnum, NULL, true, 0},

		{eNRStyleTranslateX, eNRStyleTypeString, _SC("translate-x"), &NodeStyle::ParseTransform, defaultValZero, true, 0},
		{eNRStyleTranslateY, eNRStyleTypeString, _SC("translate-y"), &NodeStyle::ParseTransform, defaultValZero, true, 0},
		{eNRStyleTransform, eNRStyleTypeString, _SC("transform"), &NodeStyle::ParseTransform, NULL, true, 0},
		
		{eNRStyleClipPath, eNRStyleTypeString, _SC("clip-path"), &NodeStyle::ParseClipData, NULL, true, 0},
		{eNRStyleMask, eNRStyleTypeString, _SC("mask"), &NodeStyle::ParseMask, NULL, true, 0},
		{eNRStyleShadow, eNRStyleTypeString, _SC("kd-svg-shadow"), &NodeStyle::ParseShadow, NULL, true, 0},

		{eNRStyleCx, eNRStyleTypeString, _SC("cx"), &NodeStyle::ParseLenOrPercent, defaultValZero, true, 0},
		{eNRStyleCy, eNRStyleTypeString, _SC("cy"), &NodeStyle::ParseLenOrPercent, defaultValZero, true, 0},
		{eNRStyleR, eNRStyleTypeString, _SC("r"), &NodeStyle::ParseLenOrPercent, defaultValZero, true, 0},

		{eNRStyleD, eNRStyleTypeString, _SC("d"), &NodeStyle::ParseString, defaultValStr, true, 0},

		{eNRStyleOverflow, eNRStyleTypeString, _SC("overflow"), &NodeStyle::ParseEnum, defaultValStr, true, 0},

		{eNRStyleFx, eNRStyleTypeString, _SC("fx"), &NodeStyle::ParseLenOrPercent, defaultValStr, false, 0},
		{eNRStyleFy, eNRStyleTypeString, _SC("fy"), &NodeStyle::ParseLenOrPercent, defaultValStr, false, 0},

		{eNRStylePreserveAspectRatio, eNRStyleTypeString, _SC("preserveaspectratio"), &NodeStyle::ParseEnum, defaultValNone, true, 0},
		{eNRStyleNinePatch, eNRStyleTypeString, _SC("nine-patch"), &NodeStyle::ParseNinePatch, defaultValNone, true, 0},

		{eNRStyleShapeRendering, eNRStyleTypeString, _SC("shape-rendering"), &NodeStyle::ParseEnum, defaultValOptimizeSpeed, true, 0},
		
		{eNRStyleEnd, eNRStyleTypeNone, NULL, NULL, NULL},
	};

	gNodeRareStyleType = sNodeRareStyleType;
	attrs = new WTF::HashMap<UINT, NodeAttrDef>;
	for (NodeRareStyleType* it = &gNodeRareStyleType[0]; eNRStyleEnd != it->style; ++it) {
		it->hash = UHGetNameHash(it->name);
		(*attrs).set(it->hash, it->style);
#if 0
			CStdString output;
			output.Format(_SC("%s:0x%x\n"), it->name, it->hash);
			OutputDebugString(output);
#endif
	}

	KDASSERT(attrs->size() == eNRStyleEnd);
	return attrs;
}

bool NodeStyle::IsStyleChangeBounding(const SQChar* strKey) {
	NodeAttrDef attrId = FindStyleIdByName(strKey);
	if (eNRStyleEnd == attrId)
		return false;

	return gNodeRareStyleType[attrId].bChangeBounding;
}

NodeAttrDef NodeStyle::FindStyleIdByName(const SQChar* strKey) {
	WTF::HashMap<UINT, NodeAttrDef>* attrs = GetStyleTypeMap();
	if (!strKey)
		return eNRStyleEnd;
	
	WTF::HashMap<UINT, NodeAttrDef>::iterator iter = attrs->find(UHGetNameHash(strKey));
	if (iter == attrs->end())
		return eNRStyleEnd;
	return iter->second;
}

void NodeStyle::SaveOriginalAttr(const SQChar* name, const SQChar* val) {
	m_allAttrs.set(UHGetNameHash(name), std::pair<CStdString, CStdString>(CStdString(name), CStdString(val)));
}

// 字体的style要单独处理。因为新的style如果不包含某个属性，则表示删除那个属性。其他UINode不存在
// 为了兼容，不能完全删除，即单独设置的属性不能删除，而是删通过style设置的
void NodeStyle::FixTextStyle(Vector<NodeAttrDef>** oldAttrDefs, WTF::Vector<NodeAttrDef>** newAttrDefs) {
	if (NULL == *newAttrDefs)
		return;

	if (NULL == *oldAttrDefs)
		*oldAttrDefs = new Vector<NodeAttrDef>();

	for (size_t i = 0; i < (*oldAttrDefs)->size(); ++i) {
		bool bFind = false;
		for (size_t j = 0; j < (*newAttrDefs)->size(); ++j) {
			if ((*oldAttrDefs)->at(i) != (*newAttrDefs)->at(j))
				continue;

			bFind = true;
			break;
		}

		if (!bFind)
			m_intAttrs.remove((*oldAttrDefs)->at(i));
	}

	UHDeletePtr(oldAttrDefs);
	*oldAttrDefs = *newAttrDefs;

#if 0
	for (int i = eNRStyleTextAnchor; i <= eNRStyleFontWeight; ++i)
		self->m_intAttrs.remove((NodeAttrDef)i);
#endif

}

bool NodeStyle::ParseStyle(NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	CStdString sAttr(strAttr);
	WTF::Vector<NodeAttrDef>* newStyleAttrs = NULL;
	
	WTF::Vector<CStdString> styleList;
	CStdString semicolon(_SC(";"));
	UHStringSplit(sAttr, semicolon, styleList);

	bool bOk = false;
	for (size_t i = 0; i < styleList.size(); ++i) {
		CStdString &strStyle = styleList[i];
		int nSplitPos = strStyle.Find(_SC(":"));
		if (nSplitPos == -1)
			continue;

		CStdString attr = strStyle.Left(nSplitPos);
		attr.MakeLower();

		NodeAttrDef styleDef = eNRStyleEnd;
		bool bSetOk = self->_SetAttr(attr, eNRStyleTypeString, 0, 0, strStyle.Mid(nSplitPos + 1), &styleDef);

		// 无论属性设置是否成功都要记录
		if (!newStyleAttrs)
			newStyleAttrs = new WTF::Vector<NodeAttrDef>();
		if (eNRStyleEnd != styleDef)
			newStyleAttrs->push_back(styleDef);

		if (!bSetOk)
			continue;
		
		bOk = true;		
	}

	if (eNE_Text == self->m_n->GetClassEnum() && eNRStyleStyle == style)
		self->FixTextStyle(&self->m_styleAttrDefs, &newStyleAttrs);

	return bOk;
}

CStdString NodeStyle::GetAttr(LPCTSTR pstrName) const {
	CStdString sName(pstrName);
	sName.MakeLower();

	UINT nameHash = UHGetNameHash(sName.GetBuffer());
	WTF::HashMap<UINT, std::pair<CStdString, CStdString> >::const_iterator iter = m_allAttrs.find(nameHash);
	if (iter != m_allAttrs.end())
		return iter->second.second;

	WTF::HashMap<UINT, NodeAttrDef>* attrs = GetStyleTypeMap();
	WTF::HashMap<UINT, NodeAttrDef>::iterator it = attrs->find(nameHash);
	if (it == attrs->end())
		return CStdString();
	return gNodeRareStyleType[it->second].defaultVal.ToString();
}

bool NodeStyle::SetAttr(const SQChar* name, NodeRareStyleTypeDef type, int intVal, float floatVal, const SQChar* strVal) {
	CStdString lowName(name);
	lowName.MakeLower(); // name要小写，val可以不小写

	return _SetAttr(lowName, type, intVal, floatVal, strVal, NULL);
}

bool NodeStyle::_SetAttr(const CStdString& lowName, NodeRareStyleTypeDef type, int intVal, float floatVal, const SQChar* strVal, NodeAttrDef* outStyle) {
	CStdString sVal(strVal);
	NodeAttrDef style = FindStyleIdByName(lowName.GetString());
	if (outStyle)
		*outStyle = style;

	if (eNRStyleD != style) // 有些属性必须区分大小写
		sVal.MakeLower();
	if (eNRStyleEnd == style) { // 如果是自定义属性，就存起来
		SaveOriginalAttr(lowName, strVal); // 存的应该是原版，而不是小写的
		return false;
	}

	if (gNodeRareStyleType[style].style != style)
		DebugBreak();

	// 查的到就查，否则就用默认值
	if (!gNodeRareStyleType[style].parseFunc(this, style, type, intVal, floatVal, sVal.GetString())) 
		return false;
	
	// 如果分析成功，则所有的属性，都存做字符串
	SaveOriginalAttr(lowName, strVal); // 存的应该是原版，而不是小写的

	if (eNRStyleId == style)
		m_id = strVal;
	
	return true;
}

bool NodeStyle::ParseColor (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString == type) {
		CStdString sAttr(strAttr);
		sAttr.MakeLower();

		int rgb = UHParseColorToRGB(sAttr);
		
		// 颜色也是存到intattr里
		return ParseInt (self, style, eNRStyleTypeInt, rgb, 0, NULL);
	}
	return false;
}

const SkMatrix* NodeStyle::GetTransform() {
	if (!m_transform) {
		static SkMatrix* transform = 0;
		if (!transform) {
			transform = new SkMatrix();
			transform->reset();
		}
		
		return transform;
	}

	return m_transform;
}

const KdPath* NodeStyle::GetClipPath() {
	if (m_clipData)
		return m_clipData;

	const KdPath* clipPath = NULL;

	// 如果有overflow属性，优先用overflow.
	if (eNRSOverflowHidden == Overflow()) {
		FloatRect overflowRect((float)X(), (float)Y(), (float)Width(), (float)Height());
		if (overflowRect.isEmpty())
			return NULL;
		
		if (!m_clipData)
			m_clipData = new KdPath();
		m_clipData->addRect(overflowRect);

		return m_clipData;
	} else 
		UHDeletePtr(&m_clipData);

	const ClipAttrData* clipAttrData = m_clipAttrData;

	if (!clipAttrData)
		return NULL;

	if (clipAttrData->IsURL()) {
		UINode* clipNode = m_n->GetManager()->GetEffectsResNodeByUrl(clipAttrData->GetUrl().GetString(), m_n);
		if (clipNode) {
			int dispatchOk = clipNode->Dispatch(_SC("GetClipPath"), 0, NULL, m_n, &clipPath);
			clipNode->deref();
			if (0 != dispatchOk)
				clipPath = NULL;
		}
	} else
		clipPath = clipAttrData->GetPath();

	return clipPath;
}

const ShadowData* NodeStyle::GetShadowData() const {
	return m_shadowData;
}

bool NodeStyle::ParseClipData (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	if (!ParseString(self, style, type, nAttr, fAttr, strAttr))
		return false;

	CStdString data(strAttr);
	data.MakeLower();
	UHDeletePtr(&self->m_clipAttrData);
	self->m_clipAttrData = new ClipAttrData();
	if (-1 != data.Find(_SC("url(#"))) {
		data.Delete(0, 5);
		data.Delete(data.GetLength() - 1, 1);
		self->m_clipAttrData->SetUrl(data);
	} else { // 目前支持"rect(0, 0, 10, 100)"的写法
		KdPath* path = self->m_clipAttrData->ToPath();
		if (!UHCalculateRectPath(data, *path)) {
			UHDeletePtr(&self->m_clipAttrData);
			return false;
		}
	}

	return true;
}

bool NodeStyle::ParseMask (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	CStdString data(strAttr);
	data.MakeLower();
	if (-1 == data.Find(_SC("url(#")))
		return false;

	data.Delete(0, 5);
	data.Delete(data.GetLength() - 1, 1);
	bool ok = ParseString(self, style, type, nAttr, fAttr, data.GetString());
	if (ok) { // 见ParseColorOrUri
		UINode* resNode = self->m_n->GetManager()->GetEffectsResNodeByUrl(data, self->m_n);
		if (resNode)
			resNode->deref();
	}

	return ok;
}

bool NodeStyle::ParseShadow (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	// kd-svg-shadow:0 0 2 RGB(0,0,0)
	CStdString data(strAttr);
	data.MakeLower();
	data = UHTrimBlankSpace(data);
	data.Replace(_SC(", "), _SC(","));
	data.Replace(_SC(" ,"), _SC(","));

	int x = 0;
	int y = 0;
	int blur = 0;

	int r = 0;
	int g = 0;
	int b = 0;
	int ret = _tsscanf(data.GetString(), _SC("%d %d %d rgb(%d,%d,%d)"), &x, &y, &blur, &r, &g, &b);
	if (6 != ret)
		return false;

	UHDeletePtr(&self->m_shadowData);
	if(!blur && !x && !y)
		return true;

	self->m_shadowData = new ShadowData(x, y, blur, 0, ShadowData::Normal, true, KdColor(r, g, b));
	return true;
}

bool NodeStyle::ParseTransform (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;
	
	if (eNRStyleTranslateX == style || eNRStyleTranslateY == style) {
		WTF::HashMap<NodeAttrDef, int>::iterator iter = self->m_intAttrs.find(style);
		int val = (int)_tcstod(strAttr, (SQChar**)NULL);
		if (iter != self->m_intAttrs.end() && iter->second == val)
			return false;

		self->m_intAttrs.set(style, val);

		if (!self->m_transform) {
			self->m_transform = new SkMatrix();
			self->m_transform->reset();
		}

		if (eNRStyleTranslateX == style)
			self->m_transform->setTranslateX(SkIntToScalar(val));
		else
			self->m_transform->setTranslateY(SkIntToScalar(val));

		return true;
	} else if (eNRStyleTransform == style) {
		float x = 0;
		float y = 0;
		float z = 0;
		CStdString sAttr(strAttr);
		sAttr.MakeLower();

		float angle = 0;
		sAttr = UHTrimBlankSpace(sAttr);
		sAttr.Trim();
		sAttr.Replace(_SC(", "), _SC(","));
		sAttr.Replace(_SC(" ,"), _SC(","));
		sAttr.Replace(_SC("( "), _SC("("));
		sAttr.Replace(_SC(" )"), _SC(")"));
		sAttr.Replace(_SC(" "), _SC(","));

		int nRet = _tsscanf(sAttr.GetString(), _SC("rotate3d(%f,%f,%f)"), &x, &y, &z);
		if (3 == nRet) {
			if (!self->m_transform) {
				self->m_transform = new SkMatrix();
				self->m_transform->reset();
			}
			//Sk3DView view;
			//view.translate(15, -25, 5);
			//view.rotateX(x);
			//view.rotateY(y);
			//view.rotateZ(z);
			//view.getMatrix(self->m_transform);
			return true;
		}
		
		nRet = _tsscanf(sAttr.GetString(), _SC("rotate(%f,%f,%f)"), &angle, &x, &y);
		if (3 == nRet) {
			if (!self->m_transform) {
				self->m_transform = new SkMatrix();
				self->m_transform->reset();
			}
			self->m_transform->setRotate(angle, x, y);
			return true;
		}

		sAttr.Replace(_SC(" "), _SC(""));
		nRet = _tsscanf(sAttr.GetString(), _SC("translate(%f,%f)"), &x, &y);
		if (2 == nRet) {
			if (!self->m_transform) {
				self->m_transform = new SkMatrix();
				self->m_transform->reset();
			}
			self->m_transform->setTranslate(x, y);
			return true;
		}

		nRet = _tsscanf(sAttr.GetString(), _SC("scale(%f,%f)"), &x, &y);
		if (2 == nRet) {
			if (!self->m_transform) {
				self->m_transform = new SkMatrix();
				self->m_transform->reset();
			}
			self->m_transform->setScale(x, y);
			return true;
		}
	}

	return false;
}

bool NodeStyle::ParseInt (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeInt == type) {
		WTF::HashMap<NodeAttrDef, int>::iterator iter = self->m_intAttrs.find(style);
		if (iter != self->m_intAttrs.end() && iter->second == nAttr)
			return false;

		self->m_intAttrs.set(style, nAttr);
		return true;
	} else if (eNRStyleTypeString == type) {
		int val = (int)UHFastStrToFloat(strAttr);
		WTF::HashMap<NodeAttrDef, int>::iterator iter = self->m_intAttrs.find(style);
		if (iter != self->m_intAttrs.end() && iter->second == val)
			return false;

		self->m_intAttrs.set(style, val);
		if (eNRStyleX == style) // 优化速度，故单独放
			self->m_x = val;
		else if (eNRStyleY == style)
			self->m_y = val;
		else if (eNRStyleWidth == style)
			self->m_width = val;
		else if (eNRStyleHeight == style)	
			self->m_height = val;

		// 如果有溢出剪裁且区域变化，则要删除久的剪切数据
		if ((eNRStyleX == style || eNRStyleY == style || eNRStyleWidth == style || eNRStyleHeight == style) &&
			eNRSOverflowHidden == self->Overflow() && self->m_clipData)
			UHDeletePtr(&self->m_clipData);
		
		return true;
	}

	return false;
}

bool NodeStyle::ParseFloat (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeFloat == type) {
		WTF::HashMap<NodeAttrDef, float>::iterator iter = self->m_floatAttrs.find(style);
		if (iter != self->m_floatAttrs.end() && iter->second == fAttr)
			return false;

		self->m_floatAttrs.set(style, fAttr);
		return true;
	} else if (eNRStyleTypeString == type) {
		float val = (float)_tcstod(strAttr, (SQChar**)NULL);
		WTF::HashMap<NodeAttrDef, float>::iterator iter = self->m_floatAttrs.find(style);
		if (iter != self->m_floatAttrs.end() && iter->second == val)
			return false;

		self->m_floatAttrs.set(style, val);
		return true;
	}
	return false;
}

bool NodeStyle::ParseId (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int i, float f, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	if (!ParseString(self, style, type, i, f, strAttr))
		return false;

	CPageManager* manager = self->m_n->GetManager();
	manager->AddToIdMap(strAttr, self->m_n);
	// 如果是资源节点，需要关联下对应的拥有者节点
	if (self->m_n->IsResNode()) {
		manager->AddEffectsResNode(self->m_n);
		manager->GetEffectsResOwnerNodes(self->m_n);
	}
	
	return true;
}

bool NodeStyle::ParseColorOrUri (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	CStdString sAttr(strAttr);
	sAttr.MakeLower();

	ColorOrUrl colorOrUrl;
	if (-1 != sAttr.Find(_SC("none"))) {
		colorOrUrl.type = eCYTNull;
	} else if (-1 != sAttr.Find(_SC("url(#"))) {
		colorOrUrl.type = eCYTUrl;
		sAttr.Delete(0, 5);
		sAttr.Delete(sAttr.GetLength() - 1, 1);
		colorOrUrl.url = sAttr;

		// 如果是URL，此时要去资源池查找一次，以便建立起资源和拥有者的联系
		// 否则如果资源是给mask用，将无机会被查找。
		// 这是因为mask下的子节点一般是不会有paint消息的。
		UINode* resNode = self->m_n->GetManager()->GetEffectsResNodeByUrl(sAttr, self->m_n);
		if (resNode)
			resNode->deref();
	} else {
		colorOrUrl.type = eCYTColor;
		colorOrUrl.color = UHParseColorToRGB(sAttr);
	}

	WTF::HashMap<NodeAttrDef, ColorOrUrl>::iterator it = self->m_ColorOrUrlAttrs.find(style);
	if (it != self->m_ColorOrUrlAttrs.end()) {
		if (it->second == colorOrUrl)
			return false;
	}
	self->m_ColorOrUrlAttrs.set(style, colorOrUrl);

	return true;
}

bool NodeStyle::ParseFontSize (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	int val = (int)_tcstod(strAttr, (SQChar**)NULL);
	WTF::HashMap<NodeAttrDef, int>::iterator it = self->m_intAttrs.find(style);
	if (it != self->m_intAttrs.end() && it->second == val)
		return false;

	self->m_intAttrs.set(style, (int)val);
	return true;
}

bool NodeStyle::ParseEnum (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	//typedef stdext::hash_map<NodeAttrDef, std::map<CStdString, int> > EnumValMapT;
	typedef WTF::HashMap<NodeAttrDef, WTF::HashMap<UINT, int> > EnumValMapT;
	static EnumValMapT* sEnumVal = NULL;
	if (!sEnumVal) { // c++表现力太弱，下面这个相当于一个脚本json结构
		sEnumVal = new EnumValMapT();

		WTF::HashMap<UINT, int> vec;
		vec.set(UHGetNameHash(_SC("inherit")), eNRSVisibilityInherit);
		vec.set(UHGetNameHash(_SC("hidden")), eNRSVisibilityHidden);
		sEnumVal->set(eNRStyleVisibility, vec);
		
		vec.clear();
		vec.set(UHGetNameHash(_SC("inherit")), eNRSPointerEventsInherit);
		vec.set(UHGetNameHash(_SC("none")), eNRSPointerEventsNone);
		sEnumVal->set(eNRStylePointerEvents, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("start")), eNRSTextAnchorStart);
		vec.set(UHGetNameHash(_SC("middle")), eNRSTextAnchorMiddle);
		vec.set(UHGetNameHash(_SC("end")), eNRSTextAnchorEnd);
		sEnumVal->set(eNRStyleTextAnchor, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("none")), eNRSTextDecorationNone);
		vec.set(UHGetNameHash(_SC("underline")), eNRSTextDecorationUnderline);
		sEnumVal->set(eNRStyleTextDecoration, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("normal")), eNRSFontWeightNormal);
		vec.set(UHGetNameHash(_SC("bold")), eNRSFontWeightBold);
		vec.set(UHGetNameHash(_SC("bolder")), eNRSTextDecorationUnderline);
		vec.set(UHGetNameHash(_SC("lighter")), eNRSFontWeightLighter);
		sEnumVal->set(eNRStyleFontWeight, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("visible")), eNRSOverflowVisible);
		vec.set(UHGetNameHash(_SC("hidden")), eNRSOverflowHidden);
		sEnumVal->set(eNRStyleOverflow, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("visible")), eNRSTextOverflowVisible);
		vec.set(UHGetNameHash(_SC("ellipsis")), eNRSTextOverflowEllipsis);
		sEnumVal->set(eNRStyleTextOverflow, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("none")), eNRSPreserveAspectRatioNone);
		vec.set(UHGetNameHash(_SC("defer")), eNRSPreserveAspectRatioDefer);
		sEnumVal->set(eNRStylePreserveAspectRatio, vec);

		vec.clear();
		vec.set(UHGetNameHash(_SC("optimizespeed")), eNRSShapeRenderingOptimizeSpeed);
		vec.set(UHGetNameHash(_SC("crispedges")), eNRSShapeRenderingCrispEdges);
		sEnumVal->set(eNRStyleShapeRendering, vec);
	}

	EnumValMapT::iterator iter = sEnumVal->find(style);
	KDASSERT(iter != sEnumVal->end());

	CStdString sAttr(strAttr);
	sAttr.MakeLower();

	WTF::HashMap<UINT, int>::iterator it = iter->second.find(UHGetStrHash(sAttr));
	if (it == iter->second.end()) // 从脚本传来的字符串，在属性对应的枚举值里找不到，则设置失败
		return false;
	
	int val = it->second;
	self->m_intAttrs.set(style, (int)val);

	if (eNRStyleOverflow == style) // 如果剪切风格变了，则删除原来的，这样下次绘制的时候会去创建
		UHDeletePtr(&self->m_clipData);

	return true;
}

bool NodeStyle::ParseNinePatch (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int, float, const SQChar* strAttr) {
	CStdString sAttr(strAttr);
	sAttr.Replace(_SC(" "), _SC(""));

	int nRet = _tsscanf(sAttr.GetString(), _SC("%d,%d"), &self->m_ninePatchX, &self->m_ninePatchY);
	if (2 == nRet)
		return true;

	self->m_ninePatchX = -1;
	self->m_ninePatchY = -1;

	return false;
}

// 字符串就存起来
bool NodeStyle::ParseString (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	CStdString sAttr(strAttr);

	bool bNoStore = false;
	if (eNRStyleSrc == style)
		bNoStore = ResCache::GetCache()->CheckNoStoreWhenResLoad(sAttr); // 在这去掉一些后缀

	if (eNRStyleTypeString != type)
		return false;

	WTF::HashMap<NodeAttrDef, CStdString>::iterator iter = self->m_CStdStringAttrs.find(style);
	if (iter != self->m_CStdStringAttrs.end() && iter->second == sAttr && !bNoStore)
		return false;

	self->m_CStdStringAttrs.set(style, sAttr);
	return true;
}

bool NodeStyle::ParseLenOrPercent (NodeStyle* self, NodeAttrDef style, NodeRareStyleTypeDef type, int nAttr, float fAttr, const SQChar* strAttr) {
	if (eNRStyleTypeString != type)
		return false;

	CStdString sAttr(strAttr);
	double val = _tcstod(strAttr, (SQChar**)NULL);

	LenOrPercent lenOrPercent;

	if (-1 != sAttr.Find(_SC("%"))) {
		val /= 100;
		lenOrPercent.type = eLOPPercent;
		lenOrPercent.u.percent = (float)val;
	} else {
		lenOrPercent.type = eLOPLen;
		lenOrPercent.u.len = (float)val;
	}

	WTF::HashMap<NodeAttrDef, LenOrPercent>::iterator iter = self->m_LenOrPercentAttrs.find(style);
	if (iter != self->m_LenOrPercentAttrs.end() && self->m_LenOrPercentAttrs.get(style) == lenOrPercent)
		return false;
	self->m_LenOrPercentAttrs.set(style, lenOrPercent);
	return true;
}

bool NodeStyle::IsStyleSetted(NodeAttrDef attrId) const {
	UINT hash = gNodeRareStyleType[attrId].hash;
	WTF::HashMap<UINT, std::pair<CStdString, CStdString> >::const_iterator iter = m_allAttrs.find(hash);
	return iter != m_allAttrs.end();
}

int NodeStyle::GetintAttrById(NodeAttrDef attrId) const {
	WTF::HashMap<NodeAttrDef, int>::const_iterator iter = m_intAttrs.find(attrId);
	if (iter == m_intAttrs.end())
		return (int)(gNodeRareStyleType[attrId].defaultVal.i);

	return iter->second;
}

float NodeStyle::GetfloatAttrById(NodeAttrDef attrId) const {
	WTF::HashMap<NodeAttrDef, float>::const_iterator iter = m_floatAttrs.find(attrId);
	if (iter == m_floatAttrs.end())
		return (float)(gNodeRareStyleType[attrId].defaultVal.f);
	return iter->second;
}

CStdString NodeStyle::GetCStdStringAttrById(NodeAttrDef attrId) const {
	WTF::HashMap<NodeAttrDef, CStdString>::const_iterator iter = m_CStdStringAttrs.find(attrId);
	if (iter == m_CStdStringAttrs.end())
		return CStdString((LPCTSTR)gNodeRareStyleType[attrId].defaultVal.s);
	return iter->second;
}

ColorOrUrl NodeStyle::GetColorOrUrlAttrById(NodeAttrDef attrId) const {
	WTF::HashMap<NodeAttrDef, ColorOrUrl>::const_iterator iter = m_ColorOrUrlAttrs.find(attrId);
	if (iter == m_ColorOrUrlAttrs.end())
		return ColorOrUrl((SkColor)gNodeRareStyleType[attrId].defaultVal.i);
	return iter->second;
}

LenOrPercent NodeStyle::GetLenOrPercentAttrById(NodeAttrDef attrId) const {
	WTF::HashMap<NodeAttrDef, LenOrPercent>::const_iterator iter = m_LenOrPercentAttrs.find(attrId);
	if (iter == m_LenOrPercentAttrs.end())
		return LenOrPercent();
	return iter->second;
}

CStdString NodeStyle::DumpAllAttrs() const {
	CStdString attr;
	WTF::HashMap<UINT, std::pair<CStdString, CStdString> >::const_iterator it = m_allAttrs.begin();
	for (; it != m_allAttrs.end(); ++it) {
		UINT type = it->first;
		if (-1 == it->second.first.Find(_SC("translate-"))) { // 为了兼容老kdgui能解析
			attr += _SC("\"");
			attr += it->second.first;
			attr += _SC("\":\"");
			attr += it->second.second;
			attr += _SC("\", ");
		} else {
			attr += _SC("\"");
			attr += it->second.first;
			attr += _SC("\":");
			attr += it->second.second;
			attr += _SC(", ");
		}
	}

	attr += _SC("\"__this__\":");
	CStdString temp;
	temp.Format(_SC("%d, "), m_n);
	attr += temp;

	attr.Replace(_SC('\\'), _SC('/'));
	return attr;
}