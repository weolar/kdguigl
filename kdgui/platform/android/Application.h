#include <jni.h>
#include <UIlib.h>
#include <wtf/StdUtil.h>

class PageManagerPublic;
typedef PageManagerPublic* KdPagePtr;

class Application {
public:
	static Application* GetInst();

	void Init(jint width, jint height);
	void MainLoop();

	CPageManager* GetPageMgr() {return m_pageMgr;}

private:
	static LRESULT WINAPI ResCallback (
		KdPagePtr pKdPagePtr,
		void* pMainContext,
		void* pPageContext,
		HWND hWnd,
		const SQChar* pURL,
		IKdGuiBuffer* pAllocate
		);

	static Application* m_inst;

	Application ();
	CPageManager* m_pageMgr;
};