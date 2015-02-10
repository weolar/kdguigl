#include <UIlib.h>
#include "UIManager.h"

#ifdef _MSC_VER
#include <tchar.h>
#include <atlwin.h>
#endif

#include <wtf/ThreadSpecific.h>
#include <wtf/UtilHelp.h>
#include <wtf/ThreadTimers.h>
#include <wtf/Vector.h>
#include <wtf/PlatformEvent.h>

#include "cache/ResCache.h"
#include "graphics/GraphicsContext.h"
#include "graphics/PlatformContextNanovg.h"
#include "Dom/Style/NodeStyle.h"
#include "dom/RootNode.h"
#include "dom/DocNode.h"

#include "ScritpMgr.h"
//#include "SysPaintMgr.h"
#include "PaintMgr.h"
#include "EffectsResNodeMgr.h"
//#include "MessageMgr.h"
#include "IdMgr.h"
#include "AnimMgr.h"
//#include "DbgMgr.h"

#include "DOMTimer.h"
#include "api/PageManagerPublic.h"
#include "DispathVmForDebug.h"

#ifdef _MSC_VER
const UINT c_uTimerID = 'Weol';
#endif
/////////////////////////////////////////////////////////////////////////////////////

CPageManager::CPageManager() 
	: m_hWndPaint(NULL)
	, m_nProcessWindowMessageCount(0)
	, m_bIsAlphaWin(FALSE)

	, m_paintMgr(new PaintMgr(this))
	, m_effectsResNodeMgr(new EffectsResNodeMgr(this))
	//, m_messageMgr(new MessageMgr(this))
	, m_scriptMgr(new ScritpMgr(this))
	, m_idMgr(new IdMgr())
	, m_DOMTimerMgr(NULL)
	, m_animMgr(new AnimMgr())
	//, m_dbgMgr(new DbgMgr(this))

	, m_pagePtr(NULL)
	, m_eState(eNoInit)
	, m_threadTimers(new ThreadTimers())
	, m_bHasSvgInit(false)
	, m_bHasJonsReady(false)
	, m_bHadNotifScriptInit(false)
	, m_selfWrap(new PageManagerPublic(this)) {
}

CPageManager::~CPageManager() {
	
}

void CPageManager::Init(HWND hWnd) {
	KDASSERT(eNoInit == m_eState);
#ifdef _MSC_VER
	ASSERT(::IsWindow(hWnd));
#endif
	m_hWndPaint = hWnd;

	InitMgrs();

	SetFocus(NULL);

	m_rootNode = new RootNode();
	InitControls(m_rootNode.get());

	m_docNode = new DocNode();
	InitControls(m_docNode.get());
#ifdef _MSC_VER
	::SetTimer(m_hWndPaint, c_uTimerID, 10, NULL);
#endif
	m_eState = eRunning;
}

void CPageManager::Uninit() {
	m_eState = eUniniting;
#ifdef _MSC_VER
	::KillTimer(m_hWndPaint, c_uTimerID);
#endif
	ScheduleAllTasks(PageManagerDelayTask::ePMDTUniniting);

	m_scriptMgr->WillCloseScript(); // 发送信号给脚本最后一次机会
	ForceStopAllAnim();
	UINode::RecursionClearAllBindAndAdditionalData(m_rootNode.get()); // 调用脚本的SvgUninit，可能会再次挂接各种监听器，所以要放到这里
	UHDeletePtr(&m_DOMTimerMgr); // 这里可能也会释放脚本里的变量
	
	ScheduleAllTasks(PageManagerDelayTask::ePMDTUniniting); // 调用WillCloseScript的时候，可能会有新的待删除节点被加入，所以这里要再次调用
	m_scriptMgr->CloseMainFunc();
	ScheduleAllTasks(PageManagerDelayTask::ePMDTUniniting);

	//m_messageMgr->Uninit();
	KDASSERT(1 == m_rootNode->GetRef());
	m_rootNode = NULL;

	m_docNode = NULL;

	m_effectsResNodeMgr->UninitResNodes();

	ScheduleAllTasks(PageManagerDelayTask::ePMDTUniniting);

	UHDeletePtr(&m_paintMgr);
	UHDeletePtr(&m_threadTimers);
	
	m_eState = eScriptDestroying;
	UHDeletePtr(&m_scriptMgr); // 一定要清理完各种别的资源再关闭脚本，否则在关闭别的资源的时候，可能还会操作脚本元素

	//UHDeletePtr(&m_messageMgr);
	UHDeletePtr(&m_effectsResNodeMgr);
	UHDeletePtr(&m_idMgr);
	//UHDeletePtr(&m_dbgMgr);
	
	m_eState = eUninit;
}

void CPageManager::InitMgrs() {
	m_paintMgr->Init();
	//m_messageMgr->Init();
	m_scriptMgr->Init();
	m_effectsResNodeMgr->Init();
	m_idMgr->Init();
	m_animMgr->Init();
	//m_dbgMgr->Init();
}

// 不能在脚本环境一初始化就调用，因为那时候可能外部还没注册回调。
// 外部可能先调用load script，再注册回调。所以我们等到脚本被加载的前夕调用
void CPageManager::NotifScriptInit() {
	if (m_callbacks.m_scriptInitCallBack && !m_bHadNotifScriptInit)
		m_callbacks.m_scriptInitCallBack(GetWrap(), NULL, GetPagePtr(), GetPaintWindow(), GetVM());
	m_bHadNotifScriptInit = true;
}

void CPageManager::PostDelayTask(PageManagerDelayTask* task) {
	m_delayTasks.append(task);
}

void CPageManager::PostAsysTask(PageManagerDelayTask* task) {
	m_asysTasks.append(task);
}

bool CPageManager::LoadSyncScriptFromSrc(const CStdString& src) {
	if (!m_scriptMgr)
		return false;
	return m_scriptMgr->LoadSyncScriptFromSrc(src.GetString());
}

class DelayLoadAsyncScriptTask : public PageManagerDelayTask {
	WTF_MAKE_FAST_ALLOCATED;
public:
	DelayLoadAsyncScriptTask(CStdString* src) {
		m_src = src;
	}

	virtual void Run(CPageManager* manager, Moment moment) {
		if (ePMDTTimer == moment)
			manager->LoadSyncScriptFromSrc(*m_src);
		UHDeletePtr(&m_src);
	}

	virtual void Destroy() {delete this;}

private:
	CStdString* m_src;
};

bool CPageManager::LoadAsyncScriptFromSrc(const CStdString& src) {
	if (!m_scriptMgr)
		return false;

	m_docURI = src;

	PostAsysTask(new DelayLoadAsyncScriptTask(new CStdString(src)));
	return true;
}

bool CPageManager::LoadScriptFromBuf(LPCTSTR src, LPCTSTR scriptBuf, int size) {
	if (!m_scriptMgr)
		return false;

	m_docURI = src;

	NotifScriptInit();
	// 编译并执行
	m_scriptMgr->CompileAndCall(scriptBuf, size, src);

	return true;
}

void CPageManager::SaveScriptDebugInfo(KdPageDebugInfo* info) {
	if (m_scriptMgr)
		m_scriptMgr->SaveScriptDebugInfo(info);
}

// 如果没有任何可视元素，强制刷新一遍，防止jsoninit回调不触发
void CPageManager::NotifSvgInited() {
	m_bHasSvgInit = true;
	InvalidateAll();
}

HWND CPageManager::GetPaintWindow() const {
	return m_hWndPaint;
}

HWND CPageManager::GetHWND() const {
	return m_hWndPaint;
}

HSQUIRRELVM CPageManager::GetVM() {
	if (!m_scriptMgr)
		return NULL;
	return m_scriptMgr->GetVM();
}

void CPageManager::UpdateLayout() {
	m_paintMgr->PostClientPaintMessage();
}

void CPageManager::Invalidate(const IntRect& rc) {
	if (eRunning != m_eState || rc.isEmpty() || rc.maxY() < 0 || rc.maxX() < 0)
		return;

	if (!rc.isEmpty())
		m_paintMgr->PostPaintMessage(&rc);
}

void CPageManager::InvalidateAll() {
	m_paintMgr->PostClientPaintMessage();
}

UINode* CPageManager::GetRoot() {
	if (m_rootNode.get())
		m_rootNode->ref();
	return m_rootNode.get();
}

UINode* CPageManager::GetDocNode() {
	if (m_docNode.get())
		m_docNode->ref();
	return m_docNode.get();
}

bool CPageManager::InitControls(UINode* pControl) {
	pControl->SetManager(this);
	pControl->SetParent(NULL);

	return true;
}

void CPageManager::ReapObjects(UINode* pControl) {
	//m_messageMgr->CheckForReapObjects(pControl);

	if (eUniniting != m_eState)
		KDASSERT(pControl != m_rootNode.get());

	ReapResources(pControl);
}

int CPageManager::GetImageResBySrc(NVGcontext* ctx, const CStdString& pSrc, UINode* owner) {
	KDASSERT(owner->GetManager() == this);
	return (ResCache::GetCache())->GetResBySrc(ctx, pSrc, owner);
}

//////////////////////////////////////////////////////////////////////////
// 转发给m_effectsResNodeMgr
UINode* CPageManager::GetEffectsResNodeByUrl(LPCTSTR pSrc, UINode* owner) {
	return m_effectsResNodeMgr->GetEffectsResNodeByUrl(pSrc, owner);
}

void CPageManager::AddEffectsResNode(UINode* resNode) {
	return m_effectsResNodeMgr->AddEffectsResNode(resNode);
}

void CPageManager::RemoveEffectsResNode(UINode* resNode) {
	return m_effectsResNodeMgr->RemoveEffectsResNode(resNode);
}

WTF::Vector<UINode*>* CPageManager::GetEffectsResOwnerNodes(UINode* resNode) {
	return m_effectsResNodeMgr->GetEffectsResOwnerNodes(resNode);
}
void CPageManager::RemoveEffectsResOwnerNode(UINode* ownerNode) {
	m_effectsResNodeMgr->RemoveEffectsResOwnerNode(ownerNode);
}

void CPageManager::ReapResources(UINode* pControl) {
	m_effectsResNodeMgr->ReapEffectsResources(pControl);
}

void CPageManager::ReapNodeTreeResources(UINode* pControl) {
	m_effectsResNodeMgr->ReapNodeTreeEffectsResources(pControl);
}
//////////////////////////////////////////////////////////////////////////

void CPageManager::AddToIdMap(const SQChar* id, UINode* n) {
	m_idMgr->AddToIdMap(id, n);
}

void CPageManager::RemoveIdMapByNode(UINode* n) {
	m_idMgr->RemoveIdMapByNode(n);
}

UINode* CPageManager::GetNodeByID(const CStdString& id) {
	return m_idMgr->GetNodeByID(id);
}

//////////////////////////////////////////////////////////////////////////

void CPageManager::AppendAnimNode(UINode* node) {
	m_animMgr->AppendAnimNode(node);
}

void CPageManager::RemoveAnimNode(UINode* node) {
	m_animMgr->RemoveAnimNode(node);
}

void CPageManager::ForceStopAllAnim() {
	m_animMgr->ForceStopAllAnim();
}

//////////////////////////////////////////////////////////////////////////

DOMTimer* CPageManager::GetDOMTimer() {
	if (!m_DOMTimerMgr)
		m_DOMTimerMgr = new DOMTimer();
	return m_DOMTimerMgr;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CPageManager::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
#ifdef _MSC_VER
	PAINTSTRUCT ps = {0};
	BeginPaint(m_hWndPaint, &ps);
	m_paintMgr->OnPaintEvent(ps.hdc, &ps.rcPaint);
	EndPaint(m_hWndPaint, &ps);
#endif
	return 0;
}

LRESULT CPageManager::OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return 0;
}

void CPageManager::SetBackgroundColor(COLORREF c) {
	if (m_paintMgr)
		m_paintMgr->m_bdColor = (RGBA32)c;
}

void CPageManager::CopyMemoryDC(HDC hDC, const RECT* rc) {
	m_paintMgr->CopyMemoryDC(hDC, rc);
}

void CPageManager::SetLayerWindow(bool b) {
	m_paintMgr->m_bUseLayeredBuffer = b;
}

void CPageManager::ScheduleTasks(Vector<PageManagerDelayTask*>& tasks, PageManagerDelayTask::Moment moment) {
	for (size_t i = 0; i < tasks.size(); ++i) { // 注意，在run的时候，delayTasks可能会变长
		PageManagerDelayTask* task = tasks[i];
		task->Run(this, moment);
		task->Destroy();
	}
	tasks.clear();
}

void CPageManager::ScheduleAllTasks(PageManagerDelayTask::Moment moment) {
	ScheduleTasks(m_delayTasks, moment);
	ScheduleTasks(m_asysTasks, moment);
}

void CPageManager::ScheduleTaskForHeartBeat(UINT uMsg) {
	if (0 != m_nProcessWindowMessageCount)
		return;

	m_nProcessWindowMessageCount++;

	sq_update_and_juage_isrinning();

	ScheduleTasks(m_delayTasks, PageManagerDelayTask::ePMDTWindowsMsg);
#ifdef _MSC_VER
	if (WM_TIMER == uMsg) 
#endif
	{
		ScheduleTasks(m_asysTasks, PageManagerDelayTask::ePMDTTimer);
		m_threadTimers->sharedTimerFired();

		m_paintMgr->SchedulePaintEvent();

	}
	
	m_nProcessWindowMessageCount--;
}

BOOL CPageManager::ProcessDbgMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
	//return m_dbgMgr->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
	return TRUE;
}

BOOL CPageManager::PreProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
	ScheduleTaskForHeartBeat(uMsg);
	
	ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);

	//if (m_messageMgr)
	//	return m_messageMgr->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);

	return FALSE;
}

void CPageManager::SetClientRectAndInvalideta(const IntRect& rc) {
	if (m_paintMgr)
		m_paintMgr->SetClientRectAndInvalideta(rc);
}

void CPageManager::SetIsDraggableRegionNcHitTest() {
	//m_messageMgr->SetIsDraggableRegionNcHitTest();
}

void CPageManager::SetFocus(UINode* pControl) {
	//m_messageMgr->SetFocus(pControl);
}

bool CPageManager::IsLayerWindow() {
	return m_paintMgr->m_bUseLayeredBuffer;
}

void CPageManager::ResetRequestRender() {
	if (m_paintMgr)
		m_paintMgr->m_bRequestRender = false;
}

bool CPageManager::IsRequestRender() {
	return m_paintMgr->m_bRequestRender;
}


DWORD nextFireTime = -2;

void CPageManager::MainLoop() {
	if (!m_paintMgr)
		return;

	double minNextFireTime = m_threadTimers->minNextFireTime();
	if (-2 != nextFireTime) {
		if (-1 == minNextFireTime)
			WTF::WaitEvent(m_paintMgr->m_hMainLoopWakeUpEvent, (DWORD)-1);
		else
			WTF::WaitEvent(m_paintMgr->m_hMainLoopWakeUpEvent, (DWORD)minNextFireTime);
	}
	nextFireTime = (DWORD)minNextFireTime;

	ScheduleTaskForHeartBeat(0);
}