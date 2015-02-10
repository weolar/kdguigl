/* see copyright notice in squirrel.h */
#include <UIlib.h>
#include <Script/include/squirrel.h>
#include <Script/include/sqstdaux.h>
#include <assert.h>

#include "script/squirrel/SqErrorHandling.h"

void _sqstd_printcallstack(HSQUIRRELVM v, SQPRINTFUNCTION pf)
{
	SQStackInfos si;
	SQInteger i;
	SQFloat f;
	const SQChar *s;
	SQInteger level=1; //1 is to skip this function that is level 0
	const SQChar *name=0; 
	SQInteger seq=0;
	pf(v,_SC("\nCALLSTACK\n"));
	while(SQ_SUCCEEDED(sq_stackinfos(v,level,&si)))
	{
		const SQChar *fn=_SC("unknown");
		const SQChar *src=_SC("unknown");
		if(si.funcname)fn=si.funcname;
		if(si.source)src=si.source;
		pf(v,_SC("*FUNCTION [%s()] %s line [%d]\n"),fn,src,si.line);
		level++;
	}
	level=0;
	pf(v,_SC("\nLOCALS\n"));

	for(level=0;level<10;level++){
		seq=0;
		while((name = sq_getlocal(v,level,seq)))
		{
			seq++;
			switch(sq_gettype(v,-1))
			{
			case OT_NULL:
				pf(v,_SC("[%s] NULL\n"),name);
				break;
			case OT_INTEGER:
				sq_getinteger(v,-1,&i);
				pf(v,_SC("[%s] %d\n"),name,i);
				break;
			case OT_FLOAT:
				sq_getfloat(v,-1,&f);
				pf(v,_SC("[%s] %.14g\n"),name,f);
				break;
			case OT_USERPOINTER:
				pf(v,_SC("[%s] USERPOINTER\n"),name);
				break;
			case OT_STRING:
				sq_getstring(v,-1,&s);
				pf(v,_SC("[%s] \"%s\"\n"),name,s);
				break;
			case OT_TABLE:
				pf(v,_SC("[%s] TABLE\n"),name);
				break;
			case OT_ARRAY:
				pf(v,_SC("[%s] ARRAY\n"),name);
				break;
			case OT_CLOSURE:
				pf(v,_SC("[%s] CLOSURE\n"),name);
				break;
			case OT_NATIVECLOSURE:
				pf(v,_SC("[%s] NATIVECLOSURE\n"),name);
				break;
			case OT_GENERATOR:
				pf(v,_SC("[%s] GENERATOR\n"),name);
				break;
			case OT_USERDATA:
				pf(v,_SC("[%s] USERDATA\n"),name);
				break;
			case OT_THREAD:
				pf(v,_SC("[%s] THREAD\n"),name);
				break;
			case OT_CLASS:
				pf(v,_SC("[%s] CLASS\n"),name);
				break;
			case OT_INSTANCE:
				pf(v,_SC("[%s] INSTANCE\n"),name);
				break;
			case OT_WEAKREF:
				pf(v,_SC("[%s] WEAKREF\n"),name);
				break;
			case OT_BOOL:{
				sq_getinteger(v,-1,&i);
				pf(v,_SC("[%s] %s\n"),name,i?_SC("true"):_SC("false"));
						 }
						 break;
			default: assert(0); break;
			}
			sq_pop(v,1);
		}
	}
}

void sqstd_printcallstack(HSQUIRRELVM v)
{
	SQPRINTFUNCTION pf = sq_getprintfunc(v);
	if(!pf)
		return;
	_sqstd_printcallstack(v, pf);
}

void sqstd_errorcallstack(HSQUIRRELVM v)
{
	SQPRINTFUNCTION ef = sq_geterrorfunc(v);
	if(!ef)
		return;
	_sqstd_printcallstack(v, ef);
}

static void _sqstd_record_params_for_raise_exception(const SQChar *sErr)
{
#ifdef _MSC_VER
    char recordParams[15*4] = {0};
    SQInteger nLen = wcslen(sErr);
    for (SQInteger i = 0; i < nLen && i < 15*4; ++i) 
        {recordParams[i] = (char)sErr[i];}
    
    RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
#endif
}

void sqstd_record_error_for_raise_exception(HSQUIRRELVM v)
{
#ifdef _MSC_VER
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

    SQRESULT result = sq_stackinfos(v, level, &si);
    if (SQ_SUCCEEDED(result) || -2 == result) {
        const SQChar* fn = _SC("unknown");
        const SQChar* src = _SC("unknown");
        if(si.funcname) 
            fn = si.funcname;
        if(si.source) 
            src = si.source;
        
        SQInteger nLen = wcslen(fn);
        for (SQInteger i = 0; i < nLen && i < 15*4 - 1; ++i)
            funcName[i] = (char)fn[i];

        nLen = wcslen(src);
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

            nLen = wcslen(fn);
            for (SQInteger i = 0; i < nLen && i < 15*4 - 1; ++i)
                funcName2[i] = (char)fn[i];
            line2 = si.line;

            //pf(v,_SC("*FUNCTION [%s()] %s line [%d]\n"),fn,src,si.line);
        }
        sprintf_s(recordParams, 1000, "%d %s %d %s %d %s", g_sq_error_result, funcName, line, funcName2, line2, srcA);
    }

    OutputDebugStringA(recordParams);
	if (IsDebuggerPresent())
		::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
    free(recordParams);
    free(srcA);
	if (IsDebuggerPresent())
		DebugBreak();
#endif
	DebugBreak();
}

static SQInteger _sqstd_aux_printerror(HSQUIRRELVM v)
{
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	if(pf && isThrowSqException()) {
		const SQChar *sErr = 0;
		if(sq_gettop(v)>=1) {
			if(SQ_SUCCEEDED(sq_getstring(v,2,&sErr))) {                
				pf(v,_SC("\nAN ERROR HAS OCCURED [%s]\n"),sErr);
			}
			else{
				pf(v,_SC("\nAN ERROR HAS OCCURED [unknown]\n"));
			}
			_sqstd_printcallstack(v, pf);

            pf(v, _SC("*script error*: %d"), g_sq_error_result);
            sqstd_record_error_for_raise_exception(v);
		}
	}
	return 0;
}

void _sqstd_compiler_error(HSQUIRRELVM v,const SQChar *sErr,const SQChar *sSource,SQInteger line,SQInteger column)
{
#ifdef _MSC_VER
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	if(pf && isThrowSqException()) {
        pf(v,_SC("%s line = (%d) column = (%d) : error %s\n"),sSource,line,column,sErr);
        pf(v,_SC("*compiler error*: %s %d %d %s"), sSource, line, g_sq_error_result, sErr);
        //ULONG_PTR RecordParams[] = {line, column};
        //RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 2, (ULONG_PTR *)RecordParams);

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
		if(IsDebuggerPresent())
			::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
        free(recordParams);
        free(sSourceA);
        free(sErrA);
		if(IsDebuggerPresent())
			DebugBreak();
	}
#endif
	LOGI("_sqstd_compiler_error:%s %s %d", sErr, sSource, line);
	DebugBreak();
}

void sqstd_seterrorhandlers(HSQUIRRELVM v)
{
	sq_setcompilererrorhandler(v,_sqstd_compiler_error);
	sq_newclosure(v,_sqstd_aux_printerror,0);
	sq_seterrorhandler(v);
}
