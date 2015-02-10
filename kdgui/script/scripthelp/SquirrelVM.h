#ifndef _SQUIRREL_VM_H_
#define _SQUIRREL_VM_H_

//#include <windows.h>

//#include "SquirrelObject.h"
class SquirrelObject;

struct SquirrelError {
	SquirrelError(HSQUIRRELVM VM);
	SquirrelError(const SQChar* s):desc(s){}
	const SQChar *desc;
};

class SquirrelVM
{
	friend class SquirrelObject;
	friend struct SquirrelError;
private:
	SquirrelVM();
	void Init(HSQUIRRELVM VM, SquirrelObject* root);
	BOOL IsInitialized(){return _VM == NULL?FALSE:TRUE;}
	void Shutdown();
	void Cleanup();
	BOOL Update(); //debugger and maybe GC later
	SquirrelObject CompileScript(const SQChar *s);
	SquirrelObject CompileBuffer(const SQChar *s);
	SquirrelObject RunScript(const SquirrelObject &o,SquirrelObject *_this = NULL);
	static void PrintFunc(HSQUIRRELVM v,const SQChar* s,...);
	static void Errorfunc(HSQUIRRELVM v,const SQChar *s,...);
	BOOL BeginCall(const SquirrelObject &func);
	BOOL BeginCall(const SquirrelObject &func,SquirrelObject &_this);
	void PushParam(const SquirrelObject &o);
	void PushParam(const SQChar *s);
	void PushParam(SQInteger n);
	void PushParam(SQFloat f);
	void PushParam(SQUserPointer up);
	void PushParamNull();
	SquirrelObject EndCall();
	SquirrelObject CreateString(const SQChar *s);
	SquirrelObject CreateTable();	
	SquirrelObject CreateArray(int size);
	SquirrelObject CreateInstance(SquirrelObject &oclass);
	SquirrelObject CreateFunction(SQFUNCTION func);
	//const SquirrelObject &GetRootTable();
	HSQUIRRELVM GetVMPtr() { return _VM; }
private:
	HSQUIRRELVM _VM;
	int _CallState;
	//SquirrelObject* _root;
};
#endif //_SQUIRREL_VM_H_