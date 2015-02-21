
#include "Application.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "wtf/MainThread.h"
#include "Core/UIManager.h"
#include "graphics/IntRect.h"
#include "Api/KdGuiApi.h"

Application* Application::m_inst = 0;

static const char g_script[] =
"var json = ["
"	[\"rect\", {\"x\":1, \"y\":20, \"width\":100, \"height\":\"127\", \"fill\":0xff1100, \"opacity\":1, \"id\":\"test1\"},],"
"	[\"path\", {\"d\":\"M20 20 C90 40 130 40 180 20 S250 60 280 20\", \"fill\":\"none\", \"stroke\":\"#0000ff\"}],"
"];\n"
""
"function SvgInit() {"
"	var alpha = 0.01;"
"	setInterval(function () {"
"		alpha += 0.01;"
"		if (alpha >= 1)"
"			alpha = 0.01;"
"		$(\"test1\").attr({\"opacity\":alpha});"
"	}, 100);"
"}\n"
"loadScriptResourceAndParseJson([], json, svgRoot());";

//////////////////////////////////////////////////////////////////////////
extern AAssetManager* g_assetMgr;

static LRESULT ReadFromAAsset(CStdString& url, IKdGuiBuffer* pAllocate) {
	LOGI("ReadFromAAsset:%s", url.GetString());

	char xx[11] = {0};

	LRESULT result = -1;
	AAsset* pAsset = AAssetManager_open(g_assetMgr, url.Right(url.GetLength() - 7), AASSET_MODE_UNKNOWN);
	if(!pAsset)
		return -1;

	LOGI("ReadFromAAsset: pAsset Is not null");

	unsigned char* pData = 0;
	int iRet = 0;
	size_t size = AAsset_getLength(pAsset);
	if(0 == size)
		goto Exit0;

	LOGI("ReadFromAAsset: pAsset:%d", size);

	pData = pAllocate->ReAlloc(size);
	iRet = AAsset_read(pAsset, pData, size);
	if(iRet <= 0) {
		pAllocate->Free();
		pData = NULL;
	}

	memcpy(xx, pData, 10);
	//LOGI("ReadFromAAsset: xx:%s", xx);

	result = 0;

Exit0:
	AAsset_close(pAsset);
	return result;
}

LRESULT Application::ResCallback (
	KdPagePtr pKdPagePtr,
	void* pMainContext,
	void* pPageContext,
	HWND hWnd,
	const SQChar* pURL,
	IKdGuiBuffer* pAllocate
	) {
	CStdString url(pURL);
	if (_SC("res:///") == url.Left(7)) {
		return ReadFromAAsset(url, pAllocate);
	}
	return -1;
}

Application::Application() {
	CPageManager* g_pageMgr = 0;
	m_inst = 0;
}

Application* Application::GetInst() {
	if (!m_inst)
		m_inst = new Application();
	return m_inst;
}

void Application::Init(jint width, jint height) {
	LOGI("Java_com_android_gl2jni_GL2JNILib_init");

	MTInit();

	CStdString url("res:///demo1.js");
	IntRect rc(0, 0, width, height);

	m_pageMgr = new CPageManager();
	m_pageMgr->Init(0);
	m_pageMgr->SetClientRectAndInvalideta(rc);
	m_pageMgr->m_callbacks.m_resHandle = &Application::ResCallback;
	//m_pageMgr->LoadScriptFromBuf("test.js", g_script, sizeof(g_script));
	m_pageMgr->LoadSyncScriptFromSrc(url);
}

///

void TestDraw();
void Application::MainLoop() {
	//TestDraw();

	m_pageMgr->MainLoop();
}

CPageManager* g_pageMgr = 0;

extern "C" JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_init(JNIEnv * env, jobject obj, jint width, jint height)
{
	//setupGraphics(width, height);
	Application::GetInst()->Init(width, height);

}

extern "C" JNIEXPORT jboolean JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_step(JNIEnv * env, jobject obj)
{
	//renderFrame();
	Application::GetInst()->MainLoop();

	return true;
}