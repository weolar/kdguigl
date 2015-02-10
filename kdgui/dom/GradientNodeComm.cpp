#include <UIlib.h>
#include "GradientNodeComm.h"
#include "Dom/Style/NodeStyle.h"
#include "Graphics/KdColor.h"
#include "Core/UIManager.h"

GradientNodeComm::GradientNodeComm() {
	m_bGradientDataNeedUpdata = true;
	m_gradientData.gradient = NULL;
}

GradientNodeComm::~GradientNodeComm() {
}

// void GradientNodeComm::SetManager(CPageManager* pManager) {
// 	if (pManager)
// 		pManager->AddResNode(this);
// 	m_pManager = pManager;
// }

bool GradientNodeComm::AppendChild(UINode* pControl) {
	bool b = UINode::AppendChild(pControl);
	if (b)
		OnSelfOrChildStateChange(eAppendChild);
	return b;
}

bool GradientNodeComm::RemoveChild(UINode* pControl) {
	bool b = UINode::RemoveChild(pControl);
	if (b)
		OnSelfOrChildStateChange(eRemoveChild);
	return b;
}

void GradientNodeComm::OnChildSetAttrBegin() {
	OnSelfOrChildStateChange(eAttrBegin);
	UINode::OnChildSetAttrBegin();
}

void GradientNodeComm::OnChildSetAttrEnd() {
	OnSelfOrChildStateChange(eAttrEnd);
	UINode::OnChildSetAttrEnd();
}

// 子节点有变化，要通知关联的拥有者节点。这在mask中可能用到
void GradientNodeComm::OnSelfOrChildStateChange(ChildState childState) {
	m_bGradientDataNeedUpdata = true;
	WTF::Vector<UINode*>* ownerNodes = m_pManager->GetEffectsResOwnerNodes(this);
	if (!ownerNodes)
		return;

	for (size_t i = 0; i < ownerNodes->size(); ++i) {
		UINode* n = ownerNodes->at(i);
		n->SetNeedLayout();
		n->OnDrawResChange(childState);
	}
}

bool GradientNodeComm::SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {
	if (!m_pManager)
		return false;

	bool b = UINode::SetAttr(pstrName, pstrValue);
	m_bGradientDataNeedUpdata = true;

	WTF::Vector<UINode*>* ownerNodes = m_pManager->GetEffectsResOwnerNodes(this);
	if (!ownerNodes)
		return true;

	for (size_t i = 0; i < ownerNodes->size(); ++i) {
		UINode* n = ownerNodes->at(i);
		n->OnChildSetAttr(pstrName, pstrValue);
		n->SetNeedLayout();
	}
	return true;
}

int GradientNodeComm::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	CStdString sFunc(func);
	if (sFunc == _SC("GetGradientData") && ret) {
		*(GradientData**)ret = CollectGradientArgByNode((UINode*)arg);
		return 0;
	}
	
	return -1;
}

GradientData* GradientNodeComm::CollectGradientArgByNodeCommEnd(UINode* owner) {
	m_gradientData.gradient->setSpreadMethod(SpreadMethodPad);

	for(size_t it = 0; it < m_children.size(); ++it) {
		UINode* n = static_cast<UINode*>(m_children[it]);
		float stopOpacity = n->GetAttrs()->StopOpacity();

		KdColor stopColor(n->GetAttrs()->StopColor());
		float offset = n->GetAttrs()->Offset().u.len;
		if (offset > 1)
			offset = 1;
		
		Gradient::ColorStop colorStop(offset, stopColor.red()/255.0f, stopColor.green()/255.0f, stopColor.blue()/255.0f, stopOpacity);
	
		m_gradientData.gradient->addColorStop(colorStop);
	}

	IntRect ownerRect = owner->BoundingRectInLocalCoordinates();

	AffineTransform gradientTransform;
	//calculateGradientTransform(gradientTransform);

	m_gradientData.userspaceTransform = gradientTransform; // 先初始化
	m_gradientData.userspaceTransform.translate(ownerRect.x(), ownerRect.y());
	m_gradientData.userspaceTransform.scaleNonUniform(ownerRect.width(), ownerRect.height());

	m_gradientData.userspaceTransform *= gradientTransform;
	m_gradientData.gradient->setGradientSpaceTransform(m_gradientData.userspaceTransform);

	m_bGradientDataNeedUpdata = false;
	return &m_gradientData;
}