#ifndef _SQUIRREL_OBJECT_H_
#define _SQUIRREL_OBJECT_H_

//#include <windows.h>
#include "../include/squirrel.h"
#include "SquirrelVM.h"

class SquirrelObject
{
	//friend class SquirrelVM;
public:
	SquirrelObject();
	virtual ~SquirrelObject();
	SquirrelObject(const SquirrelObject &o);
	SquirrelObject(HSQOBJECT &o);
	SquirrelObject & operator =(const SquirrelObject &o);
	SquirrelObject & operator =(int n);
	void Append(const SquirrelObject &o);
	void AttachToStackObject(int idx);
	SquirrelObject Clone();
	BOOL SetValue(const SquirrelObject &key,const SquirrelObject &val);
	
	BOOL SetValue(SQInteger key,const SquirrelObject &val);
	BOOL SetValue(int key,bool b);
	BOOL SetValue(int key,int n);
	BOOL SetValue(int key,float f);
	BOOL SetValue(int key,const SQChar *s);

	BOOL SetValue(const SQChar *key,const SquirrelObject &val);
	BOOL SetValue(const SQChar *key,bool b);
	BOOL SetValue(const SQChar *key,int n);
	BOOL SetValue(const SQChar *key,float f);
	BOOL SetValue(const SQChar *key,const SQChar *s);
	
	BOOL SetInstanceUP(SQUserPointer up);
	BOOL IsNull() /*const*/;
	BOOL IsNumeric() /*const*/;
	int Len() /*const*/;
	BOOL SetDelegate(SquirrelObject &obj);
	SquirrelObject GetDelegate();
	const SQChar* ToString();
	bool ToBool();
	SQInteger ToInteger();
	SQFloat ToFloat();
	SQUserPointer GetInstanceUP(unsigned int tag) /*const*/;
	SquirrelObject GetValue(const SQChar *key) /*const*/;
	BOOL Exists(const SQChar *key) /*const*/;
	float GetFloat(const SQChar *key) /*const*/;
	int GetInt(const SQChar *key) /*const*/;
	const SQChar *GetString(const SQChar *key) /*const*/;
	bool GetBool(const SQChar *key) /*const*/;
	SquirrelObject GetValue(int key) /*const*/;
	float GetFloat(int key) /*const*/;
	int GetInt(int key) /*const*/;
	const SQChar *GetString(int key) /*const*/;
	bool GetBool(int key) /*const*/;
	SquirrelObject GetAttributes(const SQChar *key = NULL);
	SQObjectType GetType();
	HSQOBJECT &GetObject(){return _o;}
	BOOL BeginIteration();
	BOOL Next(SquirrelObject &key,SquirrelObject &value);
	void EndIteration();
	HSQOBJECT _o;

private:
	BOOL GetSlot(const SQChar *name) /*const*/;
	BOOL GetSlot(int key) /*const*/;
	SquirrelVM m_Vm;
};

struct StackHandler {
	StackHandler(HSQUIRRELVM v) {
		_top = sq_gettop(v);
		this->v = v;
	}
	SQFloat GetFloat(int idx) {
		SQFloat x = 0.0f;
		if(idx > 0 && idx <= _top) {
			sq_getfloat(v,idx,&x);
		}
		return x;
	}
	SQInteger GetInt(int idx) {
		SQInteger x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getinteger(v,idx,&x);
		}
		return x;
	}
	HSQOBJECT GetObject(int idx) {
		HSQOBJECT x;
		if(idx > 0 && idx <= _top) {
			sq_resetobject(&x);
			sq_getstackobj(v,idx,&x);
		}
		return x;
	}
	const SQChar *GetString(int idx)
	{
        const SQChar *x = NULL;
		if(idx > 0 && idx <= _top) {
			sq_getstring(v,idx,&x);
		}
		return x;
	}
	SQUserPointer GetUserPointer(int idx)
	{
		SQUserPointer x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getuserpointer(v,idx,&x);
		}
		return x;
	}
	SQUserPointer GetInstanceUp(int idx,unsigned int tag)
	{
		SQUserPointer self;
		if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&self,(SQUserPointer)tag)))
			return NULL;
		return self;
	}
	SQUserPointer GetUserdata(int idx,unsigned int tag)
	{
		unsigned int otag;
		SQUserPointer up;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getuserdata(v,idx,&up,(SQUserPointer*)&otag))) {
				if(tag == otag)
					return up;
			}
		}
		return NULL;
	}
	BOOL GetBool(int idx)
	{
		SQBool ret;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getbool(v,idx,&ret)))
				return ret;
		}
		return FALSE;
	}
	int GetType(int idx)
	{
		if(idx > 0 && idx <= _top) {
			return sq_gettype(v,idx);
		}
		return -1;
	}
	
	int GetParamCount() {
		return _top;
	}
	int Return(const SQChar *s)
	{
		sq_pushstring(v,s,-1);
		return 1;
	}
	int Return(float f)
	{
		sq_pushfloat(v,f);
		return 1;
	}
	int Return(int i)
	{
		sq_pushinteger(v,i);
		return 1;
	}
	int Return(bool b)
	{
		sq_pushbool(v,b);
		return 1;
	}
	int Return(SquirrelObject &o)
	{
		sq_pushobject(v,o.GetObject());
		return 1;
	}
	int Return() { return 0; }
	int ThrowError(const SQChar *error) {
		return sq_throwerror(v,error);
	}
private:
	int _top;
	HSQUIRRELVM v;
};

#endif //_SQUIRREL_OBJECT_H_