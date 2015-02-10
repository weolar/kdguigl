
#include <UIlib.h>
#include <wtf/UtilHelp.h>
#include <wtf/RefCountedLeakCounter.h>

#include "core/UIManager.h"
#include "UINode.h"
#include "style/NodeStyle.h"
#include "EventListener.h"
#include "ListenersMgr.h"

#include "graphics/GraphicsContext.h"
#include "rendering/RenderSupport.h"

#include "NodeAnim.h"

#include "Core/ScritpMgr.h"
#include "script/include/sqstdaux.h" // for sqstd_printcallstack

#ifndef NDEBUG
#include "WTF/HashSet.h"

static RefCountedLeakCounter UINodeCounter(_SC("UINode"));
WTF::HashSet<UINode*>* g_leaksNodeSet;
WTF::Mutex g_leaksNodeSetMutex;

#endif

// 引用计数一开始为0，添加和删除子节点不影响计数，一旦deref发现为0且没父节点，则开始析构
UINode::UINode()
	: m_ref(0)
	, m_pManager(NULL)
	, m_pParent(NULL)
	, m_bVisible(true)
	, m_bFocused(false)
	, m_bEnabled(true)
	, m_kqWidget(NULL)
	, m_kqUserdata(NULL)
	, m_eState(eRunning)
	, m_nodeStyle(new NodeStyle(this))
	, m_listenerMgr(NULL)
	, m_delayLayoutTimer(NULL)
	, m_bStartDelayLayout(false)
	, m_bVisibleWhenBeginSetAttr(true)
	, m_bRepaintDirty(true)
	, m_bBoundingDirty(true) {
#ifndef NDEBUG
	UINodeCounter.increment();

	WTF::MutexLocker lock(g_leaksNodeSetMutex);
	if (!g_leaksNodeSet)
		g_leaksNodeSet = new HashSet<UINode*>();
	g_leaksNodeSet->add(this);
#endif
}

UINode::~UINode() {
#ifndef NDEBUG
	UINodeCounter.decrement();

	WTF::MutexLocker lock(g_leaksNodeSetMutex);
	g_leaksNodeSet->remove(this);
#endif

	m_eState = eUninited;

	KDASSERT(0 == m_children.size());
	if(m_pManager)
		m_pManager->ReapObjects(this);

	// 删除节点的时候，要注意清ID MAP、节点所拥有的资源，包括图片资源和字体和渐变资源
	ClearEventListener();

	UHDeletePtr(&m_listenerMgr);
	UHDeletePtr(&m_nodeStyle);
	UHDeletePtr(&m_delayLayoutTimer);

	UHDeletePtr(&m_kqWidget);
	UHDeletePtr(&m_kqUserdata);
}

static void CheckRefIsNull(UINode* n) {
	if (!IsDebuggerPresent())
		return;

	CPageManager* manager = n->GetManager();

 	int associateCount = 0;
// 	if (manager->IsRootNode(n))
// 		associateCount++;
// 	if (manager->IsFocusNode(n))
// 		associateCount++;
// 	if (manager->IsEventHoverNode(n))
// 		associateCount++;
// 	if (manager->IsEventClickNode(n))
// 		associateCount++;
// 	if (manager->IsEventKeyNode(n))
// 		associateCount++;
// 
// 	associateCount += manager->IsFireEventNode(n);
	
	if (associateCount != n->GetRef()) {
		CStdString str(_SC("以下节点销毁时仍然被引用:"));
		CStdString strTemp;
		strTemp.Format(_SC(" 0x%x"), n);

		str += n->GetAttrs()->m_id;
		str += strTemp;
		str += _SC("\n");
		OutputDebugString(str);
		DebugBreak();
	}
}

int UINode::ref() {
	if (eUniniting == m_eState)
		return m_ref;

	if (eUninited == m_eState) {
		KDASSERT(0);
		return m_ref;
	}

	m_ref++;
	return m_ref;
}

void UINode::deref() {
	if (eUniniting == m_eState)
		return;

	if (eUninited == m_eState) {
		KDASSERT(0);
		return;
	}
	
	m_ref--;
	KDASSERT(-1 <= m_ref); // 当父节点被销毁的时候，此时计数为-1

	// 如果没父节点，且引用计数等于子节点数，说明外部所有引用都没有了，可以进行删除。
	// 为零的时候即使没有父节点也不能删除，因为有可能是父节点在删除自身的时候调用到这
	if (m_ref > 0 || m_pParent)
		return;

	// 在销毁自己和子节点的时候，会引起一连串的反应，比如销毁监听器的时候会释放
	// 在调用本函数的时候，有可能是从fire event里过来，此时如果删除的是此节点或者此节点的父节点，会引起各种问题
	m_eState = eUniniting;

	ClearAllBindAndAdditionalData();

	for(size_t it = 0; it < m_children.size(); ++it) {
		UINode* child = (UINode*)m_children[it];
		child->ClearAllBindAndAdditionalData();
		m_pManager->ReapResources(child); // 在销毁子节点的时候，有可能子节点由于被脚本占用，暂时不能销毁，

		// 但那时候必须把对应的资源关联释放，否则下次添加资源的时候会重复
		child->SetParent(NULL); // 清理监听器的时候，可能会重入

		CheckRefIsNull(child); // 到这一步时，应该引用为0了
		if (0 == child->GetRef()) // 如果子节点计数不为零，说明被脚本占用。此时去deref，会引起子节点被销毁，脚本再操作就崩溃了
			child->deref(); // 虽然添加进来的时候不要ref，但释放的时候需要，以保证能销毁
	}
	m_children.clear();

	delete this;
}

class DelayDerefTask : public PageManagerDelayTask {
	WTF_MAKE_FAST_ALLOCATED;
public:
	DelayDerefTask(UINode* n) {
		m_deleteNode = n;
		m_deleteNode->ref();
	}

	virtual void Run(CPageManager* manager, Moment moment) {
		m_deleteNode->DelayDeref();
	}

	virtual void Destroy() {delete this;}

private:
	UINode* m_deleteNode;
};

void UINode::DelayDeref() {
	m_eState = eBeginToUninit;
	// 有时候，节点会被附靠在上面的消息监听器所引用。
	// 但清空监听器的工作不能在DestroyTree中进行，因为通常那时候在消息派发。所以放到这里进行
	RecursionClearAllBindAndAdditionalData(this);

	deref();
}

void UINode::RecursionClearAllBindAndAdditionalData(UINode* root) {
	// 在销毁整个节点树的时候，如果在UINode的析构里销毁Userdata，此时Userdata的v可能已经被销毁了
	// 所以要提前干一次
	root->ClearAllBindAndAdditionalData();

	WTF::Vector<UINode*>* childs = root->GetChilds();
	if (!childs)
		return;

	for (size_t i = 0; i < childs->size(); ++i) {
		UINode* n = childs->at(i);
		RecursionClearAllBindAndAdditionalData(n);
	}
}

BOOL UINode::DestroyTree() {
	m_pManager->PostDelayTask(new DelayDerefTask(this));

	m_eState = eBeginDelayUninit;
	// 删除的流程，先清理别的节点和本节点及其子节点的关联，再计数减一
	m_pManager->ReapNodeTreeResources(this); // 为了防止别的地方继续引用本节点
	// 这里不能清空事件监听器，因为这个函数一般在事件回调里调用。清空了返回就会蹦掉
	if (m_pParent)
		m_pParent->RemoveChild(this);

	//KDASSERT(m_ref == 1); // 调用本函数者占有1个引用计数

	if (0)
		sqstd_printcallstack(m_pManager->GetScriptMgr()->GetVM());	

	return TRUE;
}

void UINode::ClearAllBindAndAdditionalData() {
	SetUserdata(NULL);
	SetWidget(NULL);
	ClearEventListener();
}

void UINode::SetFocus() {
	if(m_pManager)
		m_pManager->SetFocus(this);
}

void UINode::SetToolTip(LPCTSTR pstrText) {
	m_sToolTip = pstrText;
}

CStdString UINode::GetToolTip() const {
	return m_sToolTip;
}

void UINode::Init() {
}

CPageManager* UINode::GetManager() const {
	return m_pManager;
}

void UINode::SetManager(CPageManager* pManager) {
	m_pManager = pManager;
}

void UINode::SetParent(UINode* pParent) {	
	m_pParent = pParent;
}

UINode* UINode::GetParent() const {
	if (m_pParent)
		m_pParent->ref();
	return m_pParent;
}

UINode* UINode::GetParentTemp() const {
	return m_pParent;
}

CStdString UINode::GetName() const {
	return m_sName;
}

void UINode::SetName(LPCTSTR pstrName) {
	m_sName = pstrName;
}

LPVOID UINode::GetInterface(LPCTSTR pstrName) {
	if( _tcscmp(pstrName, _SC("Control")) == 0 ) return this;
	return NULL;
}

bool UINode::IsEqualToTheClassName(LPCTSTR className) const {
	return 0 == _tcsicmp(GetClass(), className);
}

PassRefPtr<UINode> UINode::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData) {
	if (!RenderSupport::CanHittest(this))
		return NULL;

	if( (uFlags & UIFIND_HITTEST) != 0 && !RenderSupport::IsPointInNodeBoundingWithClippingArea(this, *(FloatPoint*)(pData), NULL))
		return NULL;

	// 如果是容器类型，则__FindControlByPointAndDispatchMsg需要派发消息，但不能接受焦点，也不能让查询机制在这里中断
	if (Proc)
		Proc(this, pData, pProcData);

	for(int it = m_children.size() - 1; it >= 0; --it) {
		UINode* n = static_cast<UINode*>(m_children[it]);
		RefPtr<UINode> ret = n->FindControl(Proc, pData, uFlags, pProcData);
		if (ret.get())
			return ret;
	}

	return IsContainer() ? NULL : this; // 如果是G，则走到这一步说明虽然点位于自己的范围内，但子控件没有一个是可见的
}

void UINode::Invalidate() {
	InvalidateRelativeRect(BoundingRectInLocalCoordinates(), false); // BoundingRectAbsolute
}

void UINode::InvalidateRelativeRect(const IntRect& rc, bool bForce) {
	// 一层层通知父节点，最终由根节点通知manage，这样做的目的是，如果节点在mask里，可以被拦截
	OnChildInvalidateRelativeRect(rc, bForce);
}

void UINode::InvalidateAbsoluteRect(const IntRect& rc, bool bForce) {
	OnChildInvalidateAbsoluteRect(rc, bForce);
}

void UINode::UpdateLayout() {
	if (m_pManager)
		m_pManager->UpdateLayout();
}

int UINode::DispatchEventToListeners(TEventUI& event, BOOL bCapturePhase) {
	if (!m_listenerMgr)
		return 0;

	return m_listenerMgr->DispatchEventToListeners(this, event, bCapturePhase);
}

bool UINode::AddEventListener(UIEventType eventType, PassRefPtr<EventListener> eventListener) {
	if (!m_listenerMgr)
		m_listenerMgr = new ListenersMgr();
	return m_listenerMgr->AddEventListener(this, eventType, eventListener);
}

void UINode::RemoveEventListener(UIEventType eventType, EventListener* eventListener) {
	if (!m_listenerMgr)
		return;
	m_listenerMgr->RemoveEventListener(this, m_pManager, eventType, eventListener);
}

void UINode::ClearEventListener() {
	if (!m_listenerMgr)
		return;
	m_listenerMgr->ClearEventListener(m_pManager);
	UHDeletePtr(&m_listenerMgr);
}

bool UINode::HaveListenersByEventType(UIEventType Type) {
	if (!m_listenerMgr)
		return false;
	return m_listenerMgr->HaveListenersByEventType(Type);
}

int UINode::Event(TEventUI& event, BOOL bCapturePhase) {
	RefPtr<UINode> protect = this; // 在dispath中可能把本节点销毁

	DispatchEventToListeners(event, bCapturePhase);
	// 派发给父节点的事情，统一交给外部进行，这是为了防止派发过程中脚本把父节点销毁了

	return 0;
}

IntRect UINode::BoundingRectInLocalCoordinates() {
	if (!m_bBoundingDirty)
		return m_rcItem;
	m_bBoundingDirty = false;

	if (IsContainer() && eNRSOverflowHidden != m_nodeStyle->GetintAttrById(eNRStyleOverflow))
		m_rcItem = IntRect();
	else
		m_rcItem = IntRect(m_nodeStyle->X(), m_nodeStyle->Y(), m_nodeStyle->Width(), m_nodeStyle->Height());

	for(size_t it = 0; it < m_children.size(); ++it) {
		UINode* n = static_cast<UINode*>(m_children[it]);
		// 子节点的区域应该取本地坐标，再加上变换.如果直接用BoundingRectAbsolute，里面会逐层加变换，不合理
		IntRect childBounding = n->BoundingRectInLocalCoordinates(); 
		SkRect skChildBounding = SkRect::MakeFromIRect(childBounding);
		NodeStyle* style = (NodeStyle*)n->GetAttrs();
		const SkMatrix* transform = style->GetTransform();
		if (!transform->isIdentity())
			transform->mapRect(&skChildBounding);

		m_rcItem.unite(IntRect(skChildBounding));
	}

	RenderSupport::IntersectClipPathWithResources(this, m_rcItem);

	return m_rcItem;
}

IntRect UINode::BoundingRectAbsolute() {
	SkRect rc = SkRect::MakeFromIRect(BoundingRectInLocalCoordinates());

	bool bHaveFloatTransform = false;
	const SkMatrix* transform = NULL;
	UINode* parent = this;
	while (parent) {
		const SkMatrix* transform = ((NodeStyle*)parent->GetAttrs())->GetTransform();
		if (transform->getType() > SkMatrix::kTranslate_Mask)
			bHaveFloatTransform = true;

		if (!transform->isIdentity())
			transform->mapRect(&rc);

		parent = parent->GetParentTemp();
	}
	
	SkIRect dst;
	if (bHaveFloatTransform) {
		rc.round(&dst);
		return IntRect(dst);
	}
	
	return IntRect(rc);
}

void UINode::SetBoundingDirty() {
	SetBoundingDirtyImp();

	UINode* parent = GetParentTemp();
	while (parent) {
		parent->SetBoundingDirtyImp();
		parent = parent->GetParentTemp();
	}		
}

void UINode::SetBoundingDirtyImp() {
	m_bBoundingDirty = true;
}

void UINode::SetRepaintDirty() {
	m_bRepaintDirty = true;
}

void UINode::Layout(IntRect rc) {
	Invalidate();
}

void UINode::SetNeedLayout() {
	SetBoundingDirty();
	SetRepaintDirty();
	Invalidate();
}

void UINode::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
	RenderCommBegin();

	for(size_t it = 0; it < m_children.size(); ++it)		
		m_children[it]->Paint(g, repaintRc);

	RenderCommEnd();
}

NodeAttrDef* UINode::GetCanRefreshAttrs(NodeAttrDef style) const {
	static NodeAttrDef defaultAttrs[] = {eNRStyleEnd};
	return defaultAttrs;
}

bool UINode::IsCanRefreshAttr(LPCTSTR pstrName) {
	NodeAttrDef style = NodeStyle::FindStyleIdByName(pstrName);
	if (eNRStyleEnd == style)
		return false;

	if (eNRStyleStyle == style)
		return true;

	NodeAttrDef* validAttrs = GetCanRefreshAttrs(style);
	for (int i = 0; eNRStyleEnd != validAttrs[i]; ++i) {
		if (style == validAttrs[i])
			return true;
	}
	
	return false;
}

void UINode::OnDrawResChange(ChildState childState) {
	if (m_pParent)
		m_pParent->OnDrawResChange(childState);
}

void UINode::OnChildSetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {
	if (m_pParent)
		m_pParent->OnChildSetAttr(pstrName, pstrValue);
}

void UINode::OnChildSetAttrBegin() {
	if (m_pParent)
		m_pParent->OnChildSetAttrBegin();
}

void UINode::OnChildSetAttrEnd() {
	if (m_pParent)
		m_pParent->OnChildSetAttrEnd();
}

// 为了优化速度，所以加入begin、end函数
void UINode::SetAttrBegin() {
	// 这里必须要绝对坐标。因为set attr可能会改变剪切和坐标变换
	// 在之后的刷新，如果用相对坐标再加坐标变换，结果肯定不对
	m_saveRepaintRectAbsolute = BoundingRectAbsolute();
	m_bRepaintDirty = false;
	m_bVisibleWhenBeginSetAttr = RenderSupport::CanRender(this);

	if (m_pParent)
		m_pParent->OnChildSetAttrBegin();
}

void UINode::SetAttrEnd() {
	m_bStartDelayLayout = false;

	if (m_bRepaintDirty) {// 如果属性导致元素内容改变，就需要刷新界面
		InvalidateAbsoluteRect(m_saveRepaintRectAbsolute, m_bVisibleWhenBeginSetAttr); // 先刷一边老边框
		if (m_bBoundingDirty) // 如果边框改变，刷新新边框
			Invalidate();
	}
	m_saveRepaintRectAbsolute.clear();
	
	if (m_pParent)
		m_pParent->OnChildSetAttrEnd();
}

bool UINode::SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue) {	
	if (!m_nodeStyle->SetAttr(pstrName, eNRStyleTypeString, 0, 0, pstrValue))
		return false;
	
	if (NodeStyle::IsStyleChangeBounding(pstrName)) // 如果属性引起边框改变，则需要刷新本节点和所有父节点树的边框
		SetBoundingDirty();

	if (IsCanRefreshAttr(pstrName)) // 如果是支持的属性就需要刷新界面
		m_bRepaintDirty = true;

	if (m_pParent)
		m_pParent->OnChildSetAttr(pstrName, pstrValue);

	return true;
}

void UINode::OnChildInvalidateAbsoluteRect(const IntRect& rc, bool bForce) {
	if (!RenderSupport::CanRender(this) && !bForce)
		return;

	UINode* parent = GetParentTemp();
	if (parent)
		parent->OnChildInvalidateAbsoluteRect(rc, bForce);
}

void UINode::OnChildInvalidateRelativeRect(const IntRect& rc, bool bForce) {
	if (!RenderSupport::CanRender(this) && !bForce)
		return;

 	IntRect clipRect(rc); // 要考虑本层会有剪切情况
 	RenderSupport::IntersectClipPathWithResources(this, clipRect);

	SkRect skRect = (SkRect)(clipRect); // 再考虑坐标变换
	if (!m_nodeStyle->GetTransform()->isIdentity())
 		m_nodeStyle->GetTransform()->mapRect(&skRect);
	IntRect rcInvalid(skRect);

	UINode* parent = GetParentTemp();
	if (parent)
		parent->OnChildInvalidateRelativeRect(rcInvalid, bForce);
}

CStdString UINode::GetAttr(LPCTSTR pstrName) const {
	return m_nodeStyle->GetAttr(pstrName);
}

bool UINode::AppendChild(UINode* pControl) {
	if (0)
		sqstd_printcallstack(m_pManager->GetScriptMgr()->GetVM());	

	if (!IsContainer())
		return false;

	UINode* childParent = pControl->GetParentTemp();
	if (childParent)
		return false;
		
	for(size_t it = 0; it < m_children.size(); ++it) {
		if(static_cast<UINode*>(m_children[it]) == pControl) {
			KDASSERT(FALSE);
			return false;
		}
	}

	pControl->SetParent(this);
	m_children.append(pControl);

	SetBoundingDirty();
	Invalidate();
	return true;
}

bool UINode::RemoveChild(UINode* pControl) {
	KDASSERT(IsContainer());
	for(size_t it = 0; it < m_children.size(); ++it) {
		if(static_cast<UINode*>(m_children[it]) == pControl) {
			pControl->SetBoundingDirty();
			pControl->Invalidate();
			pControl->SetParent(NULL);
			m_children.remove(it);
			return true;
		}
	}

	return false;
}

UINode* UINode::Prev() {
	UINode* pPrev = NULL;
	UINode* pParent = GetParentTemp();
	if (!pParent)
		return NULL;

	WTF::Vector<UINode*>* childs = pParent->GetChilds();
	for(size_t it = 0; it < childs->size(); ++it) {
		if (this == childs->at(it)) {
			if (0 != it) {
				pPrev = (UINode*)childs->at(it - 1);
				pPrev->ref();
				return pPrev;
			} else
				return NULL;
		}
	}

	KDASSERT(FALSE);
	return NULL;
}

UINode* UINode::Sibling() {
	UINode* pSibling = NULL;
	UINode* pParent = GetParentTemp();
	if (!pParent)
		return NULL;

	WTF::Vector<UINode*>* childs = pParent->GetChilds();
	for(size_t it = 0; it < childs->size(); ++it) {
		if (this == childs->at(it)) {
			if (it != childs->size() - 1) {
				pSibling = (UINode*)childs->at(it + 1);
				pSibling->ref();
				return pSibling;
			} else
				return NULL;
		}
	}

	KDASSERT(FALSE);
	return NULL;
}

UINode* UINode::FirstChild() {
	UINode* firstChild = NULL;

	WTF::Vector<UINode*>* childs = GetChilds();
	if (0 >= childs->size())
		return NULL;

	firstChild = (UINode*)childs->at(0);
	firstChild->ref();
	return firstChild;
}

UINode* UINode::LastChild() {
	UINode* lastChild = NULL;

	WTF::Vector<UINode*>* childs = GetChilds();
	if (0 >= childs->size())
		return NULL;

	lastChild = (UINode*)childs->at(childs->size() - 1);
	lastChild->ref();
	return lastChild;
}

UINode* UINode::FindByClass(const SQChar* value) const {
	CStdString findVal = value;
	if (findVal.IsEmpty())
		return 0;

	findVal.MakeLower();

	bool bFindClass = false;
	if (_SC('.') == findVal[0]) {
		bFindClass = true;
		findVal = findVal.Right(findVal.GetLength() - 1);
	}
	if (findVal.IsEmpty())
		return 0;

	for(size_t i = 0; i < m_children.size(); ++i) {
		UINode* n = (UINode*)m_children[i];
		bool bFind = false;
		if (bFindClass) {
			CStdString classAttr = n->GetAttrs()->GetAttr(_SC("class"));
			classAttr.MakeLower();
			if (classAttr.IsEmpty() || findVal != classAttr) 
				continue;
			bFind = true;
		} else
			bFind = findVal == n->GetClass();

		if (bFind) {
			n->ref();
			return n;
		}
	}
	return 0;
}

void UINode::SetWidget(KqNAdditionalData* widget) { 
	if (m_kqWidget) {
		m_kqWidget->Uninit(m_pManager->IsScriptDestroying());
		delete m_kqWidget;
	}
	m_kqWidget = widget;
}

KqNAdditionalData* UINode::Widget() { 
	return m_kqWidget;
}

void UINode::SetUserdata(KqNAdditionalData* userdata) { 
	if (m_kqUserdata) {
		m_kqUserdata->Uninit(m_pManager->IsScriptDestroying());
		delete m_kqUserdata;
	}
	m_kqUserdata = userdata;
}

KqNAdditionalData* UINode::Userdata() { 
	return m_kqUserdata;
}

//////////////////////////////////////////////////////////////////////////

// 新生成一个动画对象，并且判断是否需要停止所有动画对象
NodeAnim* UINode::Anim (bool stopAll) {
	ref(); // 清空动画数组的时候有可能会导致引用计数为零而析构，故这里加一
	if (stopAll)
		ForceStopAllAnimAndDestroy();

	deref();

	NodeAnim* animObj = new NodeAnim (this, m_animObjs.size()); // 这里会计数加一

	GetManager()->AppendAnimNode(this); // 这里也会计数加一
	m_animObjs.append(animObj);

	return animObj;
}

void UINode::ForceStopAllAnimAndDestroy() {
	int length = m_animObjs.size();

	ref(); // 下面的animObj->stop(true);这句，可能由于animObj是本对象，导致对象被消毁，所以先引用加1

	int i = 0;
	NodeAnim* animObj = 0;

	Vector<NodeAnim*> animObjs = m_animObjs;
	for (i = 0; i < length; ++i) {
		animObj = animObjs[i];
		if (0 != animObj) {
			// 使用了releaseKQueryWhenAnimFinish参数后，每停止一个对象，自己的引用计数会减2，所以
			// 先加一，stop后减2，相当于只减1.再自己释放剩下的
			animObj->ReleaseKQueryWhenAnimFinish(1);
			animObj->Stop(true); // 这里面会回调clearAnimWhenNoticed
		}
	}

	KDASSERT(0 == m_animObjs.size());
	deref();
}

void UINode::ClearAnimWhenNoticed (int idx) {
	if ((int)m_animObjs.size() <= idx) {
		KDASSERT(FALSE);
		return;
	}

	m_animObjs[idx] = 0;

	bool find = false;
	for (int i = 0; i < (int)m_animObjs.size(); ++i) {
		if (0 != m_animObjs[i]) 
		{ find = true; break; }
	}

	if (false == find) { // 如果所有动画都结束，则清空数组
		GetManager()->RemoveAnimNode(this);
		m_animObjs.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
CStdString UINode::Dump() {
	return DoDump(0);
}

CStdString UINode::DoDump(int nDeep) {
	int childrenSize = m_children.size();

	CStdString output;
	CStdString attrStr;

	CStdString deepBlankSpace;
	for (int i = 0; i < nDeep*3; ++i)
		deepBlankSpace += L' ';

	attrStr = GetAttrs()->DumpAllAttrs();

	CStdString temp;
	temp.Format(_SC("[\"%s\", {%s},"), GetClass(), attrStr.GetString()); // ["g", {"id":"IamId"},
	temp = deepBlankSpace + temp;
	output = temp;

	if (0 != childrenSize)
		output += _SC("\n");

	OutputDebugString(output.GetString());

	if (0 == _tcsicmp(GetClass(), _SC("text")))	{
		CStdString* text;
		if (0 == Dispatch(_SC("GetTextOfTextNode"), 0, NULL, NULL, &text) && text) {
			temp = *text;
			temp.Replace(_SC('\\'), _SC('/'));
			temp.Replace(_SC("\""), _SC("\\\""));
// 			temp = _SC("\"") + temp;
// 			temp += _SC("\"");
// 			temp = _SC("\n") + deepBlankSpace + _SC("   ") + temp;
			/*temp = */temp.Format(_SC("\"%s\"\n%s   %s"), temp.GetString(), deepBlankSpace.GetString(), temp.GetString());
			
			OutputDebugString(temp.GetString());
			output += temp;
		}
	}

	for (int i = 0; i < childrenSize; ++i) {
		UINode* n = (UINode*)m_children[i];
		output += n->DoDump(nDeep + 1);
	}

	if (0 != childrenSize)
		/*temp = */temp.Format(_SC("%s]"), deepBlankSpace.GetString());
	else
		temp = _SC("],");

	CStdString thisStr;
	thisStr.Format(_SC("//%x %d\n"), this, this);
	temp += thisStr;
	OutputDebugString(temp.GetString());
	output += temp;

	return output;
}