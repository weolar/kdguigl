#if !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)
#define AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////////////
//
//#include <atlwin.h>
//#include <hash_map>
#include <WTF/HashMap.h>
#include <WTF/Timer.h>

#include "Api/KdGuiApi.h"
#include "UIEventType.h"

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_PASSRESPONSE  0x00000008   // 过滤消息穿透层
#define UIFIND_ME_FIRST      0x80000000

class UINode;

class ResCache;
class SkBitmap;
struct NVGcontext;

class ScritpMgr;
class EffectsResNodeMgr;
class SysPaintMgr;
class PaintMgr;
class MessageMgr;
class CPageManager;
class IdMgr;
class AnimMgr;
class DbgMgr;

class ThreadTimers;
class DOMTimer;

class PageManagerPublic;

//////////////////////////////////////////////////////////////////////////

struct KWebApiCallbackSet {
	PFN_KdPageCallback m_xmlHaveFinished;
	PFN_KdPageWinMsgCallback m_msgPreCallBack;
	PFN_KdPageWinMsgCallback m_msgPostCallBack;
	PFN_KdPageCallback m_unintCallBack;
	PFN_KdPageScriptInitCallback m_scriptInitCallBack;
	PFN_KdResCallback m_resHandle;
	PFN_KdResCallback m_resOtherNameQuery;
	PFN_KdPagePaintCallback m_paint;
	PFN_KdPageError m_error;
	KWebApiCallbackSet() {
		memset(this, 0, sizeof(KWebApiCallbackSet));
	}
};

class PageManagerDelayTask {
public:
	enum Moment { // 调用时机。有些回调必须不在反初始化中调用，有些又必须在定时器中调用，如脚本加载回调
		ePMDTUniniting,
		ePMDTWindowsMsg,
		ePMDTTimer,
		ePMDTEnd,
	};

	virtual void Run(CPageManager* manager, Moment moment) = 0;
	virtual void Destroy() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CPageManager {
	WTF_MAKE_FAST_ALLOCATED;
public:
	CPageManager();
	~CPageManager();

public:
	void Init(HWND hWnd);
	void Uninit();

	void SaveScriptDebugInfo(KdPageDebugInfo* info);

	void UpdateLayout();
	void Invalidate(const IntRect& rc);
	void InvalidateAll();

	void SetClientRectAndInvalideta(const IntRect& clientRect);

	void NotifScriptInit();

	// 阻塞式，从资源加载脚本
	bool LoadSyncScriptFromSrc(const CStdString& src);
	// 非阻塞式
	bool LoadAsyncScriptFromSrc(const CStdString& src);

	// 从buf加载脚本
	bool LoadScriptFromBuf(LPCTSTR src, LPCTSTR scriptBuf, int size);

	void SetPagePtr(void* pagePtr) {m_pagePtr = pagePtr;}
	void* GetPagePtr() {return m_pagePtr;}

	void SetIsAlphaWin() {m_bIsAlphaWin = TRUE;}
	BOOL IsAlphaWin() {return m_bIsAlphaWin;}

	HWND GetPaintWindow() const;
	HWND GetHWND() const;

	bool InitControls(UINode* pControl);
	void ReapObjects(UINode* pControl);

	UINode* GetFocus() const;
	void SetFocus(UINode* pControl);

	PassRefPtr<UINode> FindControlByPointButNoDispatchMsg(FloatPoint pt, const TEventUI* event) const;
	static bool WINAPI __FindControlByPointAndDispatchMsg(UINode* node, LPVOID pData, LPVOID pProcData);

	UINode* GetRoot();
	UINode* GetRootTemporary() {return m_rootNode.get();} // 为性能考虑，增加一个不写引用计数的

	UINode* GetDocNode();
	UINode* GetDocNodeTemporary() {return m_docNode.get();}

	int GetImageResBySrc(NVGcontext* ctx, const CStdString& pSrc, UINode* owner);

	// 填充、蒙板需要
	UINode* GetEffectsResNodeByUrl(LPCTSTR pSrc, UINode* owner);
	void AddEffectsResNode(UINode* resNode);
	void RemoveEffectsResNode(UINode* resNode);
	WTF::Vector<UINode*>* GetEffectsResOwnerNodes(UINode* resNode);
	void RemoveEffectsResOwnerNode(UINode* ownerNode);
	// 在销毁子节点的时候，有可能子节点由于被脚本占用，暂时不能销毁，
	// 但那时候必须把对应的资源关联释放，否则下次添加资源的时候会重复
	void ReapResources(UINode* pControl); 
	void ReapNodeTreeResources(UINode* pControl); // 销毁整个树的资源对应，通常被UINode的析构调用

	// 引用计数会加一
	UINode* GetNodeByID(const CStdString& id);
	// 引用计数不变
	void AddToIdMap(const SQChar* id, UINode* n);
	void RemoveIdMapByNode(UINode* n);

	ScritpMgr* GetScriptMgr() {return m_scriptMgr;}
	HSQUIRRELVM GetVM();

	KWebApiCallbackSet m_callbacks;

	DOMTimer* GetDOMTimer();
	ThreadTimers* GetThreadTimers() {return m_threadTimers;}

	void AppendAnimNode(UINode* node);
	void RemoveAnimNode(UINode* node);
	void ForceStopAllAnim();

	void ScheduleTaskForHeartBeat(UINT uMsg);
	void ScheduleTasks(Vector<PageManagerDelayTask*>& delayTasks, PageManagerDelayTask::Moment moment);
	void ScheduleAllTasks(PageManagerDelayTask::Moment moment);
	void PostDelayTask(PageManagerDelayTask* task);
	void PostAsysTask(PageManagerDelayTask* task);

 	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL PreProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL ProcessDbgMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	int  OnInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool InputEventToRichEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void SetBackgroundColor(COLORREF c);

	void NotifSvgInited();
	bool HasSvgInit() {return m_bHasSvgInit;}

	bool HasJsonReady() {return m_bHasJonsReady;}
	void SetJonsReady() {m_bHasJonsReady = true;}

	PageManagerPublic* GetWrap() {return m_selfWrap;}

	void CopyMemoryDC(HDC hDC, const RECT* rc);

	CStdString DumpNode();

	void SetLayerWindow(bool b);

// 	bool IsRootNode(UINode* n) {return n == m_rootNode.get();}
// 	bool IsFocusNode(UINode* n) {return n == m_pFocus.get();}
// 	bool IsEventHoverNode(UINode* n) {return n == m_pEventHover.get();}
// 	bool IsEventClickNode(UINode* n) {return n == m_pEventClick.get();}
// 	bool IsEventKeyNode(UINode* n) {return n == m_pEventKey.get();}

	void PushFireEventNode(UINode* n); // 此节点正在派发事件，不允许删除，检测的时候也会特意放行
	void EraseFireEventNode(UINode* n);
	int IsFireEventNode(UINode* n);

	void SetIsDraggableRegionNcHitTest();

	void ResetRequestRender();
	bool IsRequestRender();

	NVGcontext* GetCanvas();

	void HighLightRing(UINode* n);

	enum State {
		eNoInit,
		eRunning,
		eUniniting,
		eScriptDestroying,
		eUninit
	};
	State GetState() {return m_eState;}

	// 在关闭脚本虚拟机的时候，可能会因为删除闭包导致删除节点。
	// 这些节点已经被移除了dom tree，但因为被闭包引用，所以并未在延迟回调里立即销毁，而是随着
	// 脚本关闭而销毁。此时脚本机已经被关闭，所以销毁节点而引发的销毁事件监听器中，不能再操作脚本元素
	bool IsScriptDestroying() {return eScriptDestroying == m_eState;}

	PaintMgr* GetPaintMgr() {return m_paintMgr;}

	CStdString m_docURI; // 调试用。可以知道当前是哪个窗口

	bool IsLayerWindow();

	void MainLoop();

	void handleTouchesBegin(int num, int ids[], float xs[], float ys[]);
	void handleTouchesMove(int num, int ids[], float xs[], float ys[]);
	void handleTouchesEnd(int num, int ids[], float xs[], float ys[]);
	void handleTouchesCancel(int num, int ids[], float xs[], float ys[]);

private:
	void InitMgrs();

#ifdef _MSC_VER
	BEGIN_MSG_MAP(CPageManager)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
	END_MSG_MAP()
#else
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {return TRUE;}
#endif // _MSC_VER

	LRESULT OnDumpNode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowHightLightRing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	RefPtr<UINode> m_rootNode;
	RefPtr<UINode> m_docNode;

	HWND m_hWndPaint;

	void* m_pagePtr;

#ifdef _MSC_VER
	friend PageManagerPublic;
#endif // _MSC_VER
	PageManagerPublic* m_selfWrap;

	PaintMgr* m_paintMgr;
	ScritpMgr* m_scriptMgr;
	MessageMgr* m_messageMgr;
	EffectsResNodeMgr* m_effectsResNodeMgr;
	IdMgr* m_idMgr;
	AnimMgr* m_animMgr;
	DOMTimer* m_DOMTimerMgr;
	DbgMgr* m_dbgMgr;

	bool m_bHasSvgInit; // 是否执行过脚本的SvgInit
	bool m_bHasJonsReady;  // 如果SvgInit已经执行完毕，则需要在渲染完成一张整图后调用，而不是立马调用这个回调

	State m_eState;

	bool m_bHadNotifScriptInit; // 是否通知过外部脚本环境初始化完毕

	BOOL m_bIsAlphaWin;

	ThreadTimers* m_threadTimers;
	
	Vector<PageManagerDelayTask*> m_delayTasks;
	// 脚本加载只能在定时器里做，因为别的消息会触发加载，导致还没出堆栈就被加载
	Vector<PageManagerDelayTask*> m_asysTasks; 

	int m_nProcessWindowMessageCount;
};

#endif // !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)

