#include <UIlib.h>
#include <wtf/UtilHelp.h>

#include "UIManager.h"
#include "bindings/ScriptDom.h"
#include "bindings/ScriptApi.h"

#ifdef _MSC_VER
#include "script/sqdbg/sqdbgserver.h"
#include "script/sqdbg/sqrdbg.h"
#endif // _MSC_VER

#include "script/include/squirrel.h"
#include "script/include/sqstdmath.h"
#include "script/include/sqstdstring.h"
#include "script/include/sqstdaux.h"

#include "ScritpMgr.h"
#include "KQueryScriptTools.h"

#pragma warning(push)  
#pragma warning(disable : 4996)  

void ScritpMgr::PrintFunc(HSQUIRRELVM v,const SQChar* s,...) {
	CStdString temp;
	va_list vl;
	va_start(vl, s);
	scvsprintf(temp.GetBuffer(2048),s, vl);
	temp.ReleaseBuffer();

	if (temp.IsEmpty() || L'\n' != temp.GetAt(temp.GetLength() - 1))
		temp += _SC(" -- print\n");
	OutputDebugString(temp);

	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	if (pManager->m_callbacks.m_error)
		pManager->m_callbacks.m_error(v, _SC("%ws"), temp.GetString());

	va_end(vl);
}

void ScritpMgr::ErrorFunc(HSQUIRRELVM v,const SQChar* s,...) {
	CStdString temp;
	va_list vl;
	va_start(vl, s);

	scvsprintf(temp.GetBuffer(2048), s, vl);
	temp.ReleaseBuffer();

	OutputDebugString(temp);

	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	if (pManager->m_callbacks.m_error)
		pManager->m_callbacks.m_error(v, _SC("%ws"), temp.GetString());

	va_end(vl);
}

#pragma warning(pop)

SQInteger _CppDebugTest_(HSQUIRRELVM v) {
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	const SQChar* id = NULL;
	sq_getstring(v, 2, &id);
	CStdString Id(id);
	//OutputDebugStringW(Id + L"\n");
	DebugBreak();
	//::PostMessage(pManager->GetHWND(), WM_CLOSE, 0, 0);
	return 0;
}

SQInteger KdGetVersionNum(HSQUIRRELVM v) {
	sq_pushinteger(v, 3);
	return 1;
}

SQInteger KLoadSyncScriptFromSrc(HSQUIRRELVM v) {
	const SQChar *src = NULL;

	if (SQ_FAILED(sq_getstring(v, 2, &src))) {
		sq_pushbool(v, SQFalse);
		return 1;
	}

	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	bool b = pManager->GetScriptMgr()->LoadSyncScriptFromSrc(src);
	sq_pushbool(v, b);
	return 1;
}

SQInteger KwSvgInit(HSQUIRRELVM v) {
	CPageManager* pManager = static_cast<CPageManager*>(sq_getforeignptr(v));
	pManager->NotifSvgInited();
	return 0;
}

//////////////////////////////////////////////////////////////////////////

ScritpMgr::ScritpMgr(CPageManager* manager) {
	KDASSERT(!!manager);
	m_manager = manager;
	m_v = NULL;
	m_remoteScriptDbg = NULL;
	m_debugInfo = NULL;
}

ScritpMgr::~ScritpMgr() {
	Uninit();
}

bool ScritpMgr::Init() {
	m_v = sq_open(1024);
	sq_setprintfunc(m_v, ScritpMgr::PrintFunc, ScritpMgr::ErrorFunc);
	sq_pushroottable(m_v);
	//sqstd_register_iolib(_VM);
	//sqstd_register_bloblib(_VM);
	sqstd_register_mathlib(m_v);
	sqstd_register_stringlib(m_v);
	sqstd_seterrorhandlers(m_v);

	sq_setforeignptr(m_v, m_manager);

	SbuRegisterGlobalFunc(m_v, KLoadSyncScriptFromSrc, _SC("KLoadSyncScriptFromSrc"));
	
	SbuRegisterGlobalFunc(m_v, Sq$, _SC("$"));
	SbuRegisterGlobalFunc(m_v, Sq$$, _SC("$$"));
	SbuRegisterGlobalFunc(m_v, SqSvgRoot, _SC("svgRoot"));
	SbuRegisterGlobalFunc(m_v, SqSvgDoc, _SC("svgDoc"));

	SbuRegisterGlobalFunc(m_v, SqSetTimeout, _SC("setTimeout"));
	SbuRegisterGlobalFunc(m_v, SqSetInterval, _SC("setInterval"));
	SbuRegisterGlobalFunc(m_v, SqClearTimeout, _SC("clearTimeout"));
	SbuRegisterGlobalFunc(m_v, SqClearInterval, _SC("clearInterval"));
	SbuRegisterGlobalFunc(m_v, SqGetTickCount, _SC("GetTickCount"));

	SbuRegisterGlobalFunc(m_v, SqGetClientCursorPos, _SC("GetClientCursorPos"));
	SbuRegisterGlobalFunc(m_v, SqGetScreenCursorPos, _SC("GetScreenCursorPos"));

	SbuRegisterGlobalFunc(m_v, SqGetAsyncKeyState, _SC("GetAsyncKeyState"));
	SbuRegisterGlobalFunc(m_v, SqGetCursorPosX, _SC("GetCursorPosX"));
	SbuRegisterGlobalFunc(m_v, SqGetCursorPosY, _SC("GetCursorPosY"));
	SbuRegisterGlobalFunc(m_v, KwSvgInit, _SC("__KwSvgInit__"));
	SbuRegisterGlobalFunc(m_v, SqIsDraggableRegionNcHitTest, _SC("KdIsDragHitTest"));

	SbuRegisterGlobalFunc(m_v, KdGetVersionNum, _SC("KdGetVersionNum"));

	SbuRegisterGlobalFunc(m_v, _CppDebugTest_, _SC("_CppDebugTest_"));

	_INIT_CLASS(m_v, KSqNode);
	_INIT_CLASS(m_v, KqEvt);
	_INIT_CLASS(m_v, KqAnim);

#if 0
	CStdValVector data;
	if (!UHLoadRes(L"file:///f:/KxE/kis_released_sp7.4_1337_fb/tools/kdgui3/kdgui/Core/KQueryScriptTools.js", m_manager, data))
		return false;
	CompileAndCall((LPCSTR)data.m_data->getMemoryBase(), data.m_data->getLength(), L"BaseToolSrc.js");
#else
	CompileAndCall((const char*)gKQueryScriptToolsSrc, sizeof(gKQueryScriptToolsSrc) - 1, _SC("BaseToolSrc.js"));
#endif
	return true;
}

void ScritpMgr::Uninit() {
	UHDeletePtr(&m_debugInfo);

	if (!m_v)
		return;
	sq_close(m_v);
	m_v = NULL;

#ifdef _MSC_VER
	if (m_remoteScriptDbg)
		sq_rdbg_shutdown(m_remoteScriptDbg);
#endif
	m_remoteScriptDbg = NULL;
}

void ScritpMgr::SaveScriptDebugInfo(KdPageDebugInfo* info) {
	KDASSERT(!m_debugInfo);
	UHDeletePtr(&m_debugInfo);
	m_debugInfo = new KdPageDebugInfo();
	*m_debugInfo = *info;
}

static CStdString GetCodeNameForDebug(const SQChar* srcCode) {
	const SQChar* pos = srcCode;
	do {pos++;} while (*pos != _SC('\n') && *pos != _SC('\r') && *pos != _SC('\0'));

#	define headComments _SC("//file:///")

	int len = pos - srcCode;
	if (len < sizeof(headComments _SC("a.js"))/sizeof(WCHAR)) 
		return _SC("");
	
	CStdString codeName(srcCode, len);
	codeName.MakeLower();
	CStdString head = codeName.Left(sizeof(headComments)/sizeof(WCHAR) - 1);
	if (head != headComments)
		return _SC("");

	codeName = codeName.Right(codeName.GetLength() - sizeof(headComments)/sizeof(WCHAR) + 1); // 去掉前面的头
	codeName.Replace(_SC('/'), _SC('\\'));

	return codeName;
}

bool ScritpMgr::CompileAndCall(const char* s, int cchMultiByte, const SQChar* sourceName) {
	// 数据只接受utf8格式
	LPCSTR utf8Data = (LPCSTR)s;
	const int nBOMHead = 3;

	if (cchMultiByte > nBOMHead && 0xef == (BYTE)utf8Data[0] && 0xbb == (BYTE)utf8Data[1] && 0xbf == (BYTE)utf8Data[2]) {// UTF BOM头
		utf8Data += nBOMHead;
		cchMultiByte -= nBOMHead;
	}

	LOGI("ScritpMgr: CompileAndCall 1:%s", sourceName);

	if (0 == cchMultiByte)
		return true;

	LOGI("ScritpMgr: CompileAndCall 2");

	const SQChar* code = utf8Data;
	int codeDataLen = cchMultiByte;
#ifdef _MSC_VER
	codeDataLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8Data, cchMultiByte, NULL, 0);  
	WTF::Vector<WCHAR> codeData;
	codeData.resize(codeDataLen + 3);
	memset(&codeData[0], 0, (codeDataLen + 3)*sizeof(wchar_t));  
	::MultiByteToWideChar(CP_UTF8, 0, utf8Data, cchMultiByte, (LPWSTR)&codeData[0], codeDataLen);
	code = &codeData[0];
#endif

	CStdString sCodeName = GetCodeNameForDebug((LPCTSTR)code);
	if (sCodeName.IsEmpty())
		sCodeName = sourceName;
	// 最好再做检查看sCodeName和sourcename是不是同个文件名

	SQRESULT hr = sq_compilebuffer(m_v, (const SQChar *)code, codeDataLen, sCodeName.GetString(), SQTrue);
	if (SQ_FAILED(hr)) {
		KDASSERT(FALSE);
		return false;
	}

	LOGI("ScritpMgr: CompileAndCall 3");

	// start the script that was previously compiled
	sq_pushroottable(m_v);
	if (SQ_FAILED(sq_call(m_v, 1, SQFalse, SQTrue))) {
		KDASSERT(FALSE);
		sq_pop(m_v, 1); // pop the compiled closure
		return false;
	}
		
	SQInteger state = sq_getvmstate(m_v);
	if (state != SQ_VMSTATE_IDLE && state != SQ_VMSTATE_RUNNING) {
		KDASSERT(FALSE);
		return false; 
	}
	sq_pop(m_v, 1); // pop the compiled closure
	
	return true;
}

bool ScritpMgr::LoadSyncScriptFromSrc(LPCTSTR pSrc) {
	KDASSERT(pSrc && 0 != pSrc[0]);

#ifdef _MSC_VER
	// 调试
	if (m_debugInfo && 0 != m_debugInfo->port && !m_remoteScriptDbg) {
		HSQREMOTEDBG rdbg = sq_rdbg_init(m_v, m_debugInfo->port, SQFalse);
		m_remoteScriptDbg = rdbg;

		sq_enabledebuginfo(m_v, SQTrue);
		if(!SQ_SUCCEEDED(sq_rdbg_waitforconnections(rdbg)))
			KDASSERT(FALSE);
	}
#endif

	CStdValVector data;
	CPageManager* pManager = m_manager;
	if (!UHLoadRes(pSrc, pManager, data))
		return false;

	return m_manager->LoadScriptFromBuf(pSrc, (LPCTSTR)data.GetBuffer(), data.GetSize());
}

void ScritpMgr::WillCloseScript() {
	HSQUIRRELVM v = GetVM();
	int top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, _SC("SvgUninit"), -1);

	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		sq_call(v, 1, 0, true);
	}
	sq_settop(v, top);
}

void ScritpMgr::CloseMainFunc() {
	HSQUIRRELVM v = GetVM();
	sq_pushroottable(v); // 关闭main函数
	sq_clear(v, 1);
}