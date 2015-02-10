
#include <UIlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef _MSC_VER
#include <tchar.h>
#endif
#include "../include/squirrel.h"
#include "../include/sqstdio.h"
#include "../include/sqstdmath.h"
#include "../include/sqstdstring.h"
#include "../include/sqstdaux.h"
#include "../include/sqstdblob.h"
#include "SquirrelVM.h"
#include "SquirrelObject.h"


//HSQUIRRELVM SquirrelVM::_VM = NULL;
//int SquirrelVM::_CallState = -1;
//SquirrelObject SquirrelVM::_root;

SquirrelError::SquirrelError(HSQUIRRELVM VM) 
{
	const SQChar *s;
	sq_getlasterror(VM);
	sq_getstring(VM,-1,&s);
	if(s) {
		desc = s;
	}else {
		desc = _SC("unknown error");
	}
}

SquirrelVM::SquirrelVM()
{
	_VM = NULL;
	_CallState = -1;
	//_root = NULL;
}

void SquirrelVM::Init(HSQUIRRELVM VM, SquirrelObject* root)
{
	_VM = VM;
	//_root = root;
	DebugBreak();
	//_VM = sq_open(1024);
	sq_setprintfunc(_VM,SquirrelVM::PrintFunc,SquirrelVM::Errorfunc);
	sq_pushroottable(_VM);
	sqstd_register_iolib(_VM);
	sqstd_register_bloblib(_VM);
	sqstd_register_mathlib(_VM);
	sqstd_register_stringlib(_VM);
	sqstd_seterrorhandlers(_VM);
	//_root->AttachToStackObject(-1);
	sq_pop(_VM,1);
	//TODO error handler compier error handler
}

BOOL SquirrelVM::Update()
{
	//update remote debugger
	return TRUE;
}

void SquirrelVM::Cleanup()
{
	//cleans the root table
	sq_pushnull(_VM);
	sq_setroottable(_VM);
}

void SquirrelVM::Shutdown()
{
	HSQUIRRELVM v = _VM;
	_VM = NULL;
	sq_close(v);
}

void SquirrelVM::PrintFunc(HSQUIRRELVM v,const SQChar* s,...)
{
	static SQChar temp[2048];
	va_list vl;
	va_start(vl, s);

#pragma warning(push)  
#pragma warning(disable : 4996) 
	scvsprintf( temp,s, vl);
#pragma warning(pop)

	va_end(vl);
}

void SquirrelVM::Errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	DebugBreak();
// 	va_list vl;
// 	va_start(vl, s);
// 	vfwprintf(stderr, s, vl);
// 	va_end(vl);
}

SquirrelObject SquirrelVM::CompileScript(const SQChar *s)
{
#define MAX_EXPANDED_PATH 1023
	SquirrelObject ret;
	if(SUCCEEDED(sqstd_loadfile(_VM,s,1))) {
		ret.AttachToStackObject(-1);
		sq_pop(_VM,1);
		return ret;
	}
	DebugBreak();
}

SquirrelObject SquirrelVM::CompileBuffer(const SQChar *s)
{
	SquirrelObject ret;
	if(SUCCEEDED(sq_compilebuffer(_VM,s,(int)_tcslen(s)*sizeof(SQChar),_SC("console buffer"),1))) {
		ret.AttachToStackObject(-1);
		sq_pop(_VM,1);
		return ret;
	}
	DebugBreak();
}

SquirrelObject SquirrelVM::RunScript(const SquirrelObject &o,SquirrelObject *_this)
{
	SquirrelObject ret;
	sq_pushobject(_VM,o._o);
	if(_this) {
		sq_pushobject(_VM,_this->_o);
	}
	else {
		sq_pushroottable(_VM);
	}
	if(SQ_SUCCEEDED(sq_call(_VM,1, SQFalse, SQTrue))) {
		ret.AttachToStackObject(-1);
		sq_pop(_VM,1);
		return ret;
	}
	sq_pop(_VM,1);
	DebugBreak();
}


BOOL SquirrelVM::BeginCall(const SquirrelObject &func)
{
	if(_CallState != -1)
		return FALSE;
	_CallState = 1;
	sq_pushobject(_VM,func._o);
	sq_pushroottable(_VM);
	return TRUE;
}

BOOL SquirrelVM::BeginCall(const SquirrelObject &func,SquirrelObject &_this)
{
	if(_CallState != -1)
		DebugBreak();
		//throw SquirrelError(_SC("call already initialized"));
	_CallState = 1;
	sq_pushobject(_VM,func._o);
	sq_pushobject(_VM,_this._o);
	return TRUE;
}

#define _CHECK_CALL_STATE \
	if(_CallState == -1) \
		DebugBreak();
		//throw SquirrelError(_SC("call not initialized"));

void SquirrelVM::PushParam(const SquirrelObject &o)
{
	_CHECK_CALL_STATE
	sq_pushobject(_VM,o._o);
	_CallState++;
}

void SquirrelVM::PushParam(const SQChar *s)
{
	_CHECK_CALL_STATE
	sq_pushstring(_VM,s,-1);
	_CallState++;
}

void SquirrelVM::PushParam(SQInteger n)
{
	_CHECK_CALL_STATE
	sq_pushinteger(_VM,n);
	_CallState++;
}

void SquirrelVM::PushParam(SQFloat f)
{
	_CHECK_CALL_STATE
	sq_pushfloat(_VM,f);
	_CallState++;
}

void SquirrelVM::PushParamNull()
{
	_CHECK_CALL_STATE
	sq_pushnull(_VM);
	_CallState++;
}

void SquirrelVM::PushParam(SQUserPointer up)
{
	_CHECK_CALL_STATE
	sq_pushuserpointer(_VM,up);
	_CallState++;
}

SquirrelObject SquirrelVM::EndCall()
{
	SquirrelObject ret;
	if(_CallState >= 0) { 
		int oldtop = sq_gettop(_VM);
		int nparams = _CallState;
		_CallState = -1;
		if(SQ_SUCCEEDED(sq_call(_VM,nparams, SQFalse, SQTrue))) {
			ret.AttachToStackObject(-1);
			sq_pop(_VM,2);
		}else {
			sq_settop(_VM,oldtop-(nparams+1));
			DebugBreak();
		}
		
	}
	return ret;
}

SquirrelObject SquirrelVM::CreateInstance(SquirrelObject &oclass)
{
	SquirrelObject ret;
	int oldtop = sq_gettop(_VM);
	sq_pushobject(_VM,oclass._o);
	if(SQ_FAILED(sq_createinstance(_VM,-1))) {
		sq_settop(_VM,oldtop);
		DebugBreak();
	}
	ret.AttachToStackObject(-1);
	sq_pop(_VM,2);
	return ret;
}

SquirrelObject SquirrelVM::CreateTable()
{
	SquirrelObject ret;
	sq_newtable(_VM);
	ret.AttachToStackObject(-1);
	sq_pop(_VM,1);
	return ret;
}

SquirrelObject SquirrelVM::CreateString(const SQChar *s)
{
	SquirrelObject ret;
	sq_pushstring(_VM,s,-1);
	ret.AttachToStackObject(-1);
	sq_pop(_VM,1);
	return ret;
}


SquirrelObject SquirrelVM::CreateArray(int size)
{
	SquirrelObject ret;
	sq_newarray(_VM,size);
	ret.AttachToStackObject(-1);
	sq_pop(_VM,1);
	return ret;
}

SquirrelObject SquirrelVM::CreateFunction(SQFUNCTION func)
{
	SquirrelObject ret;
	sq_newclosure(_VM,func,0);
	ret.AttachToStackObject(-1);
	sq_pop(_VM,1);
	return ret;
}

// const SquirrelObject &SquirrelVM::GetRootTable()
// {
//	   return *_root;
// }