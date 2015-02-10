
#include <UIlib.h>
#include "squirrelobject.h"
#include "SquirrelVM.h"

SquirrelObject::SquirrelObject(void)
{
	 sq_resetobject(&_o);
}

SquirrelObject::~SquirrelObject()
{
	if(m_Vm.GetVMPtr())
		sq_release(m_Vm.GetVMPtr(),&_o);
}

SquirrelObject::SquirrelObject(const SquirrelObject &o)
{
	_o = o._o;
	sq_addref(m_Vm.GetVMPtr(),&_o);
}

SquirrelObject::SquirrelObject(HSQOBJECT &o)
{
	_o = o;
	sq_addref(m_Vm.GetVMPtr(),&_o);
}

SquirrelObject SquirrelObject::Clone()
{
	SquirrelObject ret;
	if(GetType() == OT_TABLE || GetType() == OT_ARRAY)
	{
		sq_pushobject(m_Vm.GetVMPtr(),_o);
		sq_clone(m_Vm.GetVMPtr(),-1);
		ret.AttachToStackObject(-1);
		sq_pop(m_Vm.GetVMPtr(),2);
	}
	return ret;

}

SquirrelObject & SquirrelObject::operator =(const SquirrelObject &o)
{
	HSQOBJECT t;
	t = o._o;
	sq_addref(m_Vm.GetVMPtr(),&t);
	sq_release(m_Vm.GetVMPtr(),&_o);
	_o = t;
	return *this;
}

SquirrelObject & SquirrelObject::operator =(int n)
{
	sq_pushinteger(m_Vm.GetVMPtr(),n);
	AttachToStackObject(-1);
	sq_pop(m_Vm.GetVMPtr(),1);
	return *this;
}

void SquirrelObject::Append(const SquirrelObject &o)
{
	if(sq_isarray(_o)) {
		sq_pushobject(m_Vm.GetVMPtr(),_o);
		sq_pushobject(m_Vm.GetVMPtr(),o._o);
		sq_arrayappend(m_Vm.GetVMPtr(),-2);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
}

void SquirrelObject::AttachToStackObject(int idx)
{
	HSQOBJECT t;
	sq_getstackobj(m_Vm.GetVMPtr(),idx,&t);
	sq_addref(m_Vm.GetVMPtr(),&t);
	sq_release(m_Vm.GetVMPtr(),&_o);
	_o = t;
}

BOOL SquirrelObject::SetDelegate(SquirrelObject &obj)
{
	if(obj.GetType() == OT_TABLE ||
		obj.GetType() == OT_NULL) {
			switch(_o._type) {
				case OT_USERDATA:
				case OT_TABLE:
					sq_pushobject(m_Vm.GetVMPtr(),_o);
					sq_pushobject(m_Vm.GetVMPtr(),obj._o);
					if(SQ_SUCCEEDED(sq_setdelegate(m_Vm.GetVMPtr(),-2)))
						return TRUE;
					break;
			}
		}
	return FALSE;
}

SquirrelObject SquirrelObject::GetDelegate()
{
	SquirrelObject ret;
	if(_o._type == OT_TABLE || _o._type == OT_USERDATA)
	{
		sq_pushobject(m_Vm.GetVMPtr(),_o);
		sq_getdelegate(m_Vm.GetVMPtr(),-1);
		ret.AttachToStackObject(-1);
		sq_pop(m_Vm.GetVMPtr(),2);
	}
	return ret;
}

BOOL SquirrelObject::IsNull() /*const*/
{
	return sq_isnull(_o);
}

BOOL SquirrelObject::IsNumeric() /*const*/
{
	return sq_isnumeric(_o);
}

int SquirrelObject::Len() /*const*/
{
	int ret = 0;
	if(sq_isarray(_o) || sq_istable(_o) || sq_isstring(_o)) {
		sq_pushobject(m_Vm.GetVMPtr(),_o);
		ret = sq_getsize(m_Vm.GetVMPtr(),-1);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	return ret;
}

#define _SETVALUE_INT_BEGIN \
	BOOL ret = FALSE; \
	int top = sq_gettop(m_Vm.GetVMPtr()); \
	sq_pushobject(m_Vm.GetVMPtr(),_o); \
	sq_pushinteger(m_Vm.GetVMPtr(),key);

#define _SETVALUE_INT_END \
	if(SQ_SUCCEEDED(sq_rawset(m_Vm.GetVMPtr(),-3))) { \
		ret = TRUE; \
	} \
	sq_settop(m_Vm.GetVMPtr(),top); \
	return ret;

BOOL SquirrelObject::SetValue(int key,const SquirrelObject &val)
{
	_SETVALUE_INT_BEGIN
	sq_pushobject(m_Vm.GetVMPtr(),val._o);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(int key,int n)
{
	_SETVALUE_INT_BEGIN
	sq_pushinteger(m_Vm.GetVMPtr(),n);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(int key,float f)
{
	_SETVALUE_INT_BEGIN
	sq_pushfloat(m_Vm.GetVMPtr(),f);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(int key,const SQChar *s)
{
	_SETVALUE_INT_BEGIN
	sq_pushstring(m_Vm.GetVMPtr(),s,-1);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(int key,bool b)
{
	_SETVALUE_INT_BEGIN
	sq_pushbool(m_Vm.GetVMPtr(),b);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(const SquirrelObject &key,const SquirrelObject &val)
{
	BOOL ret = FALSE;
	int top = sq_gettop(m_Vm.GetVMPtr());
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_pushobject(m_Vm.GetVMPtr(),key._o);
	sq_pushobject(m_Vm.GetVMPtr(),val._o);
	if(SQ_SUCCEEDED(sq_rawset(m_Vm.GetVMPtr(),-3))) {
		ret = TRUE;
	}
	sq_settop(m_Vm.GetVMPtr(),top);
	return ret;
}

#define _SETVALUE_STR_BEGIN \
	BOOL ret = FALSE; \
	int top = sq_gettop(m_Vm.GetVMPtr()); \
	sq_pushobject(m_Vm.GetVMPtr(),_o); \
	sq_pushstring(m_Vm.GetVMPtr(),key,-1);

#define _SETVALUE_STR_END \
	if(SQ_SUCCEEDED(sq_rawset(m_Vm.GetVMPtr(),-3))) { \
		ret = TRUE; \
	} \
	sq_settop(m_Vm.GetVMPtr(),top); \
	return ret;

BOOL SquirrelObject::SetValue(const SQChar *key,const SquirrelObject &val)
{
	_SETVALUE_STR_BEGIN
	sq_pushobject(m_Vm.GetVMPtr(),val._o);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,int n)
{
	_SETVALUE_STR_BEGIN
	sq_pushinteger(m_Vm.GetVMPtr(),n);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,float f)
{
	_SETVALUE_STR_BEGIN
	sq_pushfloat(m_Vm.GetVMPtr(),f);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,const SQChar *s)
{
	_SETVALUE_STR_BEGIN
	sq_pushstring(m_Vm.GetVMPtr(),s,-1);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,bool b)
{
	_SETVALUE_STR_BEGIN
	sq_pushbool(m_Vm.GetVMPtr(),b);
	_SETVALUE_STR_END
}


SQObjectType SquirrelObject::GetType()
{
	return _o._type;
}

BOOL SquirrelObject::GetSlot(int key) /*const*/
{
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_pushinteger(m_Vm.GetVMPtr(),key);
	if(SQ_SUCCEEDED(sq_get(m_Vm.GetVMPtr(),-2))) {
		return TRUE;
	}
	
	return FALSE;
}


SquirrelObject SquirrelObject::GetValue(int key)/*const*/
{
	SquirrelObject ret;
	if(GetSlot(key)) {
		ret.AttachToStackObject(-1);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

float SquirrelObject::GetFloat(int key) /*const*/
{
	float ret = 0.0f;
	if(GetSlot(key)) {
		sq_getfloat(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

int SquirrelObject::GetInt(int key) /*const*/
{
	int ret = 0;
	if(GetSlot(key)) {
		sq_getinteger(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

const SQChar *SquirrelObject::GetString(int key) /*const*/
{
	const SQChar *ret = NULL;
	if(GetSlot(key)) {
		sq_getstring(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

bool SquirrelObject::GetBool(int key) /*const*/
{
	SQBool ret = FALSE;
	if(GetSlot(key)) {
		sq_getbool(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret?true:false;
}

BOOL SquirrelObject::Exists(const SQChar *key) /*const*/
{
	BOOL ret = FALSE;
	if(GetSlot(key)) {
		ret = TRUE;
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL SquirrelObject::GetSlot(const SQChar *name) /*const*/
{
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_pushstring(m_Vm.GetVMPtr(),name,-1);
	if(SQ_SUCCEEDED(sq_get(m_Vm.GetVMPtr(),-2))) {
		return TRUE;
	}
	
	return FALSE;
}

SquirrelObject SquirrelObject::GetValue(const SQChar *key)/*const*/
{
	SquirrelObject ret;
	if(GetSlot(key)) {
		ret.AttachToStackObject(-1);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

float SquirrelObject::GetFloat(const SQChar *key) /*const*/
{
	float ret = 0.0f;
	if(GetSlot(key)) {
		sq_getfloat(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

int SquirrelObject::GetInt(const SQChar *key) /*const*/
{
	int ret = 0;
	if(GetSlot(key)) {
		sq_getinteger(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

const SQChar *SquirrelObject::GetString(const SQChar *key) //const
{
	const SQChar *ret = NULL;
	if(GetSlot(key)) {
		sq_getstring(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret;
}

bool SquirrelObject::GetBool(const SQChar *key) //const
{
	SQBool ret = FALSE;
	if(GetSlot(key)) {
		sq_getbool(m_Vm.GetVMPtr(),-1,&ret);
		sq_pop(m_Vm.GetVMPtr(),1);
	}
	sq_pop(m_Vm.GetVMPtr(),1);
	return ret?true:false;
}

SQUserPointer SquirrelObject::GetInstanceUP(unsigned int tag) //const
{
	SQUserPointer up = NULL;
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_getinstanceup(m_Vm.GetVMPtr(),-1,(SQUserPointer*)&up,(SQUserPointer)tag);
	sq_pop(m_Vm.GetVMPtr(),1);
	return up;
}

BOOL SquirrelObject::SetInstanceUP(SQUserPointer up)
{
	if(!sq_isinstance(_o)) return FALSE;
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_setinstanceup(m_Vm.GetVMPtr(),-1,up);
	sq_pop(m_Vm.GetVMPtr(),1);
	return TRUE;
}

SquirrelObject SquirrelObject::GetAttributes(const SQChar *key)
{
	SquirrelObject ret;
	int top = sq_gettop(m_Vm.GetVMPtr());
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	if(key)
		sq_pushstring(m_Vm.GetVMPtr(),key,-1);
	else
		sq_pushnull(m_Vm.GetVMPtr());
	if(SQ_SUCCEEDED(sq_getattributes(m_Vm.GetVMPtr(),-2))) {
		ret.AttachToStackObject(-1);
	}
	sq_settop(m_Vm.GetVMPtr(),top);
	return ret;
}

BOOL SquirrelObject::BeginIteration()
{
	if(!sq_istable(_o) && !sq_isarray(_o) && !sq_isclass(_o))
		return FALSE;
	sq_pushobject(m_Vm.GetVMPtr(),_o);
	sq_pushnull(m_Vm.GetVMPtr());
	return TRUE;
}

BOOL SquirrelObject::Next(SquirrelObject &key,SquirrelObject &val)
{
	if(SQ_SUCCEEDED(sq_next(m_Vm.GetVMPtr(),-2))) {
		key.AttachToStackObject(-2);
		val.AttachToStackObject(-1);
		sq_pop(m_Vm.GetVMPtr(),2);
		return TRUE;
	}
	return FALSE;
}

const SQChar* SquirrelObject::ToString()
{
	return sq_objtostring(&_o);
}

SQInteger SquirrelObject::ToInteger()
{
	return sq_objtointeger(&_o);
}

SQFloat SquirrelObject::ToFloat()
{
	return sq_objtofloat(&_o);
}

bool SquirrelObject::ToBool()
{
	//<<FIXME>>
	return _o._unVal.nInteger?true:false;
}

void SquirrelObject::EndIteration()
{
	sq_pop(m_Vm.GetVMPtr(),2);
}