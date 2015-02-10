#ifndef UINode_h
#define UINode_h

#include "UINodeEnum.h"
#include <wtf/Timer.h>
#include "Core/UIEventType.h"
#include "style/NodeAttrDef.h"

class UINode;
class GraphicsContext;
class NodeStyle;
class EventListener;
class ListenersMgr;
class NodeAnim;

typedef bool (WINAPI* FINDCONTROLPROC)(UINode*, LPVOID, LPVOID);

class KqNAdditionalData {
public:
	virtual void Uninit(bool bIsScriptDestroying) = 0;
};

class UINode {
public:
	UINode();

protected:
	virtual ~UINode();

public:
	virtual int ref();
	virtual void deref();
	int GetRef() {return m_ref;}

	void DelayDeref();

	virtual BOOL DestroyTree();

	static void RecursionClearAllBindAndAdditionalData(UINode* root);
	void ClearAllBindAndAdditionalData();

	virtual CStdString GetName() const;
	virtual void SetName(LPCTSTR pstrName);
	virtual LPVOID GetInterface(LPCTSTR pstrName);

	bool IsEqualToTheClassName(LPCTSTR className) const;

	// 多态接口，子类可以实现任意功能。
	virtual int Dispatch(LPCTSTR func, int nArg, LPCTSTR strArg, void* arg, void* ret) {return -1;}

	virtual CStdString GetToolTip() const;
	virtual void SetToolTip(LPCTSTR pstrText);

	virtual void SetFocus();

	virtual BOOL IsContainer() const {return FALSE;}

	virtual PassRefPtr<UINode> FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, LPVOID pProcData);

	virtual CPageManager* GetManager() const;
	virtual void SetManager(CPageManager* pManager);

	virtual bool IsResNode() const {return false;}

	// 注意，凡涉及到DOM节点的获取，引用计数都会加一，所以使用完要记得释放
	virtual void SetParent(UINode* pParent);
	virtual UINode* GetParent() const;
	virtual UINode* GetParentTemp() const; // 不增加引用计数。为了优化

	// 唯一例外的是这个函数，不会加一
	WTF::Vector<UINode*>* GetChilds() {return &m_children;}

	virtual bool AppendChild(UINode* pControl);
	virtual bool RemoveChild(UINode* pControl);

	virtual UINode* Prev();
	virtual UINode* Sibling();
	virtual UINode* FirstChild();
	virtual UINode* LastChild();

	void SetWidget(KqNAdditionalData* widget);
	KqNAdditionalData* Widget();

	void SetUserdata(KqNAdditionalData* userdata);
	KqNAdditionalData* Userdata();

	UINode* FindByClass(const SQChar* value) const;

	virtual void Paint(GraphicsContext* g, const IntRect &rcInvalid);

	// 不带坐标变换的外边框，但子节点带坐标变换
	virtual IntRect BoundingRectInLocalCoordinates();
	// 相对根节点的绝对坐标
	virtual IntRect BoundingRectAbsolute();
	
	virtual void Layout(IntRect rc);
	virtual void SetNeedLayout();

	virtual void Invalidate();
	// bForce的意思是，即使本节点被隐藏了，依然要刷新
	virtual void InvalidateRelativeRect(const IntRect& rc, bool bForce);
	// 刷新矩形，提交的是绝对坐标
	virtual void InvalidateAbsoluteRect(const IntRect& rc, bool bForce);

	virtual void OnChildInvalidateRelativeRect(const IntRect& rc, bool bForce);
	virtual void OnChildInvalidateAbsoluteRect(const IntRect& rc, bool bForce);

	void UpdateLayout();

	virtual void Init();
	virtual int Event(TEventUI& event, BOOL bCapturePhase);
	virtual int DispatchEventToListeners(TEventUI& event, BOOL bCapturePhase);

	bool AddEventListener(UIEventType eventType, PassRefPtr<EventListener> eventListener);
	void RemoveEventListener(UIEventType eventType, EventListener* eventListener);
	void ClearEventListener();
	bool HaveListenersByEventType(UIEventType Type);

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const;
	bool IsCanRefreshAttr(LPCTSTR pstrName);
		
	virtual const NodeStyle* GetAttrs() const {return m_nodeStyle;}
	virtual bool SetAttr(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual CStdString GetAttr(LPCTSTR pstrName) const;

	virtual void SetAttrBegin();
	virtual void SetAttrEnd();

	virtual void OnChildSetAttrBegin();
	virtual void OnChildSetAttrEnd();
	virtual void OnChildSetAttr(LPCTSTR pstrName, LPCTSTR pstrValue);

	// 关联的绘图资源变化，如渐变填充的各种属性变化
	enum ChildState {
		eAttrBegin,
		eAttrEnd,
		eAppendChild,
		eRemoveChild,
	};
	virtual void OnDrawResChange(ChildState childState);
	
	bool HasChildren() {return m_children.size() > 0;}

	virtual LPCTSTR GetClass() const = 0;
	virtual UINodeEnum GetClassEnum() const = 0;

	virtual void SetBoundingDirty();
	virtual void SetRepaintDirty();

	void ClearAnimWhenNoticed (int idx);
	void ForceStopAllAnimAndDestroy();
	NodeAnim* Anim (bool bStopAll);

	// 设置后，在反初始化过程中不允许再接受消息
	void SetStateBeginUninit() {m_eState = eBeginDelayUninit;}

	CPageManager* GetManager() {return m_pManager;}

	CStdString Dump();

protected:
	void SetBoundingDirtyImp();

	int m_ref;
	NodeStyle* m_nodeStyle;
	CPageManager* m_pManager;
	UINode* m_pParent;

	CStdString m_sName;
	CStdString m_sToolTip;

	IntRect m_rcItem;
	bool m_bBoundingDirty;
	bool m_bRepaintDirty;
	bool m_bVisibleWhenBeginSetAttr; // 当开始设置属性的时候，是否为显示状态，这样才能在结束设置，刷新的时候，不会被优化掉这次刷新
	bool m_bStartDelayLayout;
	IntRect m_saveRepaintRectAbsolute; // setAttr时候改变

	bool m_bVisible;
	bool m_bEnabled;
	bool m_bFocused;

	KqNAdditionalData* m_kqWidget;
	KqNAdditionalData* m_kqUserdata;

	WTF::Vector<UINode*> m_children;
	enum State {
		eRunning,
		eBeginDelayUninit, // 延迟反初始化的过程中，不需要再加减计数了
		eBeginToUninit,
		eUniniting,
		eUninited
	};
	State m_eState; // release时候用来防止重入

	friend class ListenersMgr;
	ListenersMgr* m_listenerMgr;

	Timer<UINode>* m_delayLayoutTimer;

	Vector<NodeAnim*> m_animObjs;

	CStdString DoDump(int nDeep);
};

#endif // UINode_h