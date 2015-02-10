#include <UIlib.h>
#include "ClipPathNode.h"
#include "Core/UIManager.h"
#include "Dom/Style/NodeStyle.h"
#include "Graphics/KdPath.h"
#include <wtf/UtilHelp.h>

ClipPathNode::ClipPathNode() {
	m_path = new KdPath();
	m_bChildrenHasUpdata = true;
}

ClipPathNode::~ClipPathNode() {
	UHDeletePtr(&m_path);
}

void ClipPathNode::SetManager(CPageManager* pManager) {
	if (pManager)
		pManager->AddEffectsResNode(this);
	m_pManager = pManager;
}

void ClipPathNode::OnChildSetAttrBegin() {
	OnSelfOrChildStateChange();
}

void ClipPathNode::OnChildSetAttrEnd() {
	OnSelfOrChildStateChange();
}

bool ClipPathNode::OnSelfOrChildStateChange() {
	m_bChildrenHasUpdata = true;

	if (!m_pManager)
		return false;

	WTF::Vector<UINode*>* ownerNodes = m_pManager->GetEffectsResOwnerNodes(this);
	if (!ownerNodes)
		return true;

	// 子节点绘图消息被转发到拥有者节点上
	for (size_t i = 0; i < ownerNodes->size(); ++i)
		ownerNodes->at(i)->SetNeedLayout();
	return true;
}

bool ClipPathNode::SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {
	bool b = UINode::SetAttr(pstrName, pstrValue);
	if (!OnSelfOrChildStateChange()) // 自己属性被改变，和子节点属性改变走的流程一样
		return false;
	return b;
}

int ClipPathNode::Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {
	CStdString sFunc(func);
	if (sFunc == _SC("ChildrenPathHasChange")) {
		OnSelfOrChildStateChange();
		return 0;
	} else if (sFunc == _SC("GetClipPath") && ret) {
		if (m_bChildrenHasUpdata)
			CollectPathByChildren((UINode*)arg);
		
		*(KdPath**)ret = m_path;
		return 0;
	}
	
	return -1;
}

const KdPath* ClipPathNode::CollectPathByChildren(UINode* owner) {
	if (!m_bChildrenHasUpdata)
		return m_path;

	m_path->clear();
	for(size_t it = 0; it < m_children.size(); ++it) {
		UINode* n = static_cast<UINode*>(m_children[it]);
		const KdPath* path;
		if (0 == n->Dispatch(_SC("GetPath"), 0, NULL, &path, NULL) && path) {
			const SkMatrix* transform = ((NodeStyle*)n->GetAttrs())->GetTransform();
			if (!transform->isIdentity()) {
				KdPath pathDummy = *path; // 因为有可能会有坐标变换
				pathDummy.transform(*transform);
				m_path->addPath(pathDummy);	
			} else
				m_path->addPath(*path);			
		}
	}

	m_bChildrenHasUpdata = false;
	return m_path;
}