
#include <UIlib.h>
#include <Script/include/squirrel.h>
#include "SqErrorHandling.h"
// #include "WebKit/kd/Api/KdGuiApi.h"

int g_sq_error_result = 0;
int g_sq_error_handling = 1;

bool isThrowSqException()
{
	return IsDebuggerPresent() || 1 == g_sq_error_handling;
}

#if 0

#define KdInfoc 1

#ifdef KdInfoc
#pragma warning(push)
#pragma warning(disable : 4996) 
#include <tchar.h>
#include <shlwapi.h>
#include "../../../../include/infoc/KInfocWrap.h"
#pragma warning(pop)
#endif

static void ReportError(LPCWSTR errortype, LPCSTR error, int version)
{
#ifdef KdInfoc
#ifdef NDEBUG
	KInfocClientWrap infoc;
	infoc.SetTableName(L"duba_kdgui_error");
	infoc.AddString(L"errortype", errortype);
	infoc.AddString(L"error", WTF::String(error).charactersWithNullTermination());
	infoc.AddInt(L"version", 0);
	infoc.WriteCache();
#endif
#endif
}

static void ReportErrorW(LPCWSTR errortype, LPCWSTR error, int version)
{
#ifdef KdInfoc && NDEBUG
#ifdef NDEBUG	
	KInfocClientWrap infoc;
	infoc.SetTableName(L"duba_kdgui_error");
	infoc.AddString(L"errortype", errortype);
	infoc.AddString(L"error", error);
	infoc.AddInt(L"version", 0);
	infoc.WriteCache();
#endif
#endif
}

static void RaiseScriptError(HSQUIRRELVM v, int errorCode)
{
	SQStackInfos si = {0};
	SQInteger level = 1; //1 is to skip this function that is level 0
	const SQChar* name = 0;
	char funcName[15*4] = {0};
	char funcName2[15*4] = {0};
	char* srcA = (char*)malloc(1001);
	SQInteger line = 0;
	SQInteger line2 = 0;
	char* recordParams = (char*)malloc(1001);
	memset(recordParams, 0, 1001);
	memset(srcA, 0, 1001);

	const SQChar* sErr = 0;
	if(!SQ_SUCCEEDED(sq_getstring(v, 2, &sErr)))
		sErr = 0;
	WTF::String sErrA(sErr);

	SQRESULT result = sq_stackinfos(v, level, &si);
	if (SQ_SUCCEEDED(result) || -2 == result) {
		const SQChar* fn = _SC("unknown");
		const SQChar* src = _SC("unknown");
		if(si.funcname) 
			fn = si.funcname;
		if(si.source) 
			src = si.source;

		SQInteger nLen = (SQInteger)wcslen(fn);
		for (SQInteger i = 0; i < nLen && i < 15*4 - 1; ++i)
			funcName[i] = (char)fn[i];

		nLen = (SQInteger)wcslen(src);
		for (SQInteger i = 0; i < nLen && i < 1000; ++i)
			srcA[i] = (char)src[i];
		const int nSaveLen = 20;
		if (nLen > nSaveLen) {
			memmove(srcA, srcA + nLen - nSaveLen, nSaveLen);
			srcA[nSaveLen] = 0;
		}

		line = si.line;

		if (SQ_SUCCEEDED(sq_stackinfos(v, 2, &si))) {
			fn = _SC("unknown");
			src = _SC("unknown");
			if(si.funcname) 
				fn = si.funcname;

			nLen = (SQInteger)wcslen(fn);
			for (SQInteger i = 0; i < nLen && i < 15*4 - 1; ++i)
				funcName2[i] = (char)fn[i];
			line2 = si.line;
		}
		sprintf_s(recordParams, 1000, "%d %s %d %s %d %s %s", errorCode, funcName, line, funcName2, line2, srcA, sErrA.utf8().data());
	}

	OutputDebugStringA(recordParams);

	ReportError(L"script", recordParams, 0);

	//MyRaiseException(recordParams, KKdGetCallStackInfo(v));
	//::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
	free(recordParams);
	free(srcA);
	if (IsDebuggerPresent())
		DebugBreak();
}

void RaiseCompilerError(HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column) {
	char* recordParams = (char*)malloc(1001);
	memset(recordParams, 0, 1001);

	char* sSourceA = (char*)malloc(1001);
	memset(sSourceA, 0, 1001);

	char* sErrA = (char*)malloc(1001);
	memset(sErrA, 0, 1001);

	int nLen = wcslen(sSource);
	int i = 0;
	int j = nLen > 13 ? nLen - 13 : 0;
	for (i = 0; j < nLen; ++i, ++j)
		sSourceA[i] = (char)sSource[j];

	nLen = wcslen(sErr);
	for (i = 0; i < nLen; ++i)
		sErrA[i] = (char)sErr[i];

	sprintf_s(recordParams, 1000, "%d %s %s", line, sSourceA, sErrA);

	OutputDebugStringA(recordParams);
	ReportError(L"compiler", recordParams, 0);
	//::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
	free(recordParams);
	free(sSourceA);
	free(sErrA);
	if (IsDebuggerPresent())
		DebugBreak();
}

void RaiseLoadScriptError(HSQUIRRELVM v, const SQChar* codeName, int errorResult)
{
	char* recordParams = (char*)malloc(1001);
	memset(recordParams, 0, 1001);

	char* codeNameA = (char*)malloc(1001);
	memset(codeNameA, 0, 1001);

	int nLen = wcslen(codeName);
	for (SQInteger i = 0; i < nLen && i < 1000; ++i)
		codeNameA[i] = (char)codeName[i];

	const int nSaveLen = 20;
	if (nLen > nSaveLen) {
		memmove(codeNameA, codeNameA + nLen - nSaveLen, nSaveLen);
		codeNameA[nSaveLen] = 0;
	}

	sprintf_s(recordParams, 1000, "%d %s", errorResult, codeNameA);

	OutputDebugStringA(recordParams);
	ReportError(L"load-script", recordParams, 0);
	//::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
	free(recordParams);
	free(codeNameA);
	if (IsDebuggerPresent())
		DebugBreak();
}

void RaiseLoadResError(KdPagePtr pagePtr, const WCHAR* filaName, const WCHAR* documentURI)
{
	WTF::String sFilaName(filaName);
	sFilaName.makeLower();
	sFilaName.replace(".svg", "");
	sFilaName = sFilaName.right(15);

	WTF::String sDocumentURI(documentURI);
	sDocumentURI.makeLower();
	sDocumentURI.replace(".svg", "");
	sDocumentURI = sDocumentURI.right(15);

	WTF::String outPut;
	outPut = outPut.format("%ws %ws", sFilaName.charactersWithNullTermination(), sDocumentURI.charactersWithNullTermination());

	OutputDebugStringW(outPut.charactersWithNullTermination());
	//::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
	ReportErrorW(L"res", outPut.charactersWithNullTermination(), 0);

	if (IsDebuggerPresent())
		DebugBreak();
}

int WINAPI KdPageError(void* arg, const WCHAR* s, ...) {
	va_list vl;
	va_start(vl, s);

	int ret = 0;
	if (0 != wcsstr(s, L"*compiler error*")) {
		const SQChar *sSource = va_arg(vl, const SQChar *);
		SQInteger line = va_arg(vl, SQInteger);
		SQInteger column = va_arg(vl, SQInteger);
		const SQChar *sErr = va_arg(vl, const SQChar *);
		RaiseCompilerError((HSQUIRRELVM)arg, sErr, sSource, line, column);
	} else if (0 != wcsstr(s, L"*script error*")) {
		int sqErrorCode;
		sqErrorCode = va_arg(vl, int);
		RaiseScriptError((HSQUIRRELVM)arg, sqErrorCode);
	} else if (0 != wcsstr(s, L"*load script error*")) {
		int errorResult = va_arg(vl, int);
		const SQChar* codeName = va_arg(vl, const SQChar*);
		RaiseLoadScriptError((HSQUIRRELVM)arg, codeName, errorResult);
	} else if (0 != wcsstr(s, L"*load res error*")) {
		const WCHAR* filaName = va_arg(vl, const WCHAR*);
		const WCHAR* documentURI = va_arg(vl, const WCHAR*);
		RaiseLoadResError((KdPagePtr)arg, filaName, documentURI);
		ret = -2; // ±Ì æ≤ª±¿¿£
	}

	va_end(vl);

	return ret;
}
#endif