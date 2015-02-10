#ifndef _SQUIRREL_VM_H_
#define _SQUIRREL_VM_H_

#include "squirrel.h"

struct SquirrelError {
	SquirrelError(HSQUIRRELVM v);
	SquirrelError(HSQUIRRELVM v,const SQChar* s):desc(s){}
	const SQChar *desc;
};

class SquirrelVM
{
	friend class SquirrelObject;
	friend struct SquirrelError;
public:
    static SquirrelVM* Create();
    ~SquirrelVM();
	/*static*/ void Init();
	/*static*/ BOOL IsInitialized(){return _VM == NULL?FALSE:TRUE;}
	/*static*/ void Shutdown();
	/*static*/ void Cleanup();
	/*static*/ BOOL Update(); //debugger and maybe GC later
	/*static*/ SquirrelObject CompileScript(const SQChar *s);
	/*static*/ SquirrelObject CompileBuffer(const SQChar *s);
	/*static*/ SquirrelObject RunScript(const SquirrelObject &o,SquirrelObject *_this = NULL);
	static void PrintFunc(HSQUIRRELVM v,const SQChar* s,...);
    static void ErrorFunc(HSQUIRRELVM v,const SQChar* s,...);
	/*static*/ BOOL BeginCall(const SquirrelObject &func);
	/*static*/ BOOL BeginCall(const SquirrelObject &func,SquirrelObject &_this);
	/*static*/ void PushParam(const SquirrelObject &o);
	/*static*/ void PushParam(const SQChar *s);
	/*static*/ void PushParam(SQInteger n);
	/*static*/ void PushParam(SQFloat f);
	/*static*/ void PushParam(SQUserPointer up);
	/*static*/ void PushParamNull();
	/*static*/ SquirrelObject EndCall();
	/*static*/ SquirrelObject CreateString(const SQChar *s);
	/*static*/ SquirrelObject CreateTable();	
	/*static*/ SquirrelObject CreateArray(int size);
	/*static*/ SquirrelObject CreateInstance(SquirrelObject &oclass);
	/*static*/ SquirrelObject CreateFunction(SQFUNCTION func);
	/*static*/ const SquirrelObject * GetRootTable();
	/*static*/ HSQUIRRELVM getScriptVM() { return _VM; }
private:
    SquirrelVM() {};

	/*static*/ HSQUIRRELVM _VM;
	/*static*/ int _CallState;
	/*static*/ SquirrelObject* _root;
};
#endif //_SQUIRREL_VM_H_