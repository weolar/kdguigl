
#ifndef ScritpMgr_h
#define ScritpMgr_h

class CPageManager;
typedef struct _KdPageDebugInfo KdPageDebugInfo;
typedef struct SQDbgServer* HSQREMOTEDBG;

#include "script/include/squirrel.h"
//#include "script/sqdbg/sqdbgserver.h"
//#include "script/sqdbg/sqrdbg.h"

class ScritpMgr {
	WTF_MAKE_FAST_ALLOCATED;
public:
	ScritpMgr(CPageManager* manager);
	~ScritpMgr();

	bool Init();
	void Uninit();
	void WillCloseScript();
	void CloseMainFunc();

	void SaveScriptDebugInfo(KdPageDebugInfo* info);

	bool LoadSyncScriptFromSrc(LPCTSTR pSrc);

	bool CompileAndCall(const char *s, int cchMultiByte, const SQChar *sourceName);

	static void PrintFunc(HSQUIRRELVM v, const SQChar* s, ...);
	static void ErrorFunc(HSQUIRRELVM v, const SQChar* s, ...);

	HSQUIRRELVM GetVM() {return m_v;}
	HSQREMOTEDBG GetScriptDbg() {return m_remoteScriptDbg;}

protected:
	HSQUIRRELVM m_v;
	CPageManager* m_manager;

	KdPageDebugInfo* m_debugInfo;
	HSQREMOTEDBG m_remoteScriptDbg;
};

#endif // ScritpMgr_h