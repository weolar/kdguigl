
#include "squirrel.h"
#include "squirrelobject.h"
#include "SquirrelVM.h"

SquirrelObject::SquirrelObject(HSQUIRRELVM v)
{
    _VM = v;
    sq_resetobject(&_o);
}

// SquirrelObject::SetVM(HSQUIRRELVM v)
// {
//     _VM = v;
// }

SquirrelObject::~SquirrelObject()
{
	if(_VM)
		sq_release(_VM,&_o);
}

SquirrelObject::SquirrelObject(const SquirrelObject &o)
{
    _VM = o._VM;
	_o = o._o;
	sq_addref(_VM,&_o);
}

SquirrelObject::SquirrelObject(HSQUIRRELVM v,HSQOBJECT &o)
{
    _VM = v;
	_o = o;
	sq_addref(_VM,&_o);
}

SquirrelObject SquirrelObject::Clone()
{
	SquirrelObject ret(_VM);
	if(GetType() == OT_TABLE || GetType() == OT_ARRAY)
	{
		sq_pushobject(_VM,_o);
		sq_clone(_VM,-1);
		ret.AttachToStackObject(-1);
		sq_pop(_VM,2);
	}
	return ret;

}

SquirrelObject & SquirrelObject::operator = (const SquirrelObject &o)
{
	HSQOBJECT t;
	t = o._o;
    _VM = o._VM;
	sq_addref(_VM,&t);
	sq_release(_VM,&_o);
	_o = t;
	return *this;
}

// SquirrelObject & SquirrelObject::operator =(int n)
// {
// 	sq_pushinteger(_VM,n);
// 	AttachToStackObject(-1);
// 	sq_pop(_VM,1);
// 	return *this;
// }

void SquirrelObject::Append(const SquirrelObject &o)
{
	if(sq_isarray(_o)) {
		sq_pushobject(_VM,_o);
		sq_pushobject(_VM,o._o);
		sq_arrayappend(_VM,-2);
		sq_pop(_VM,1);
	}
}

void SquirrelObject::AttachToStackObject(int idx)
{
	HSQOBJECT t;
	sq_getstackobj(_VM,idx,&t);
	sq_addref(_VM,&t);
	sq_release(_VM,&_o);
	_o = t;
}

BOOL SquirrelObject::SetDelegate(SquirrelObject &obj)
{
	if(obj.GetType() == OT_TABLE ||
		obj.GetType() == OT_NULL) {
			switch(_o._type) {
				case OT_USERDATA:
				case OT_TABLE:
					sq_pushobject(_VM,_o);
					sq_pushobject(_VM,obj._o);
					if(SQ_SUCCEEDED(sq_setdelegate(_VM,-2)))
						return TRUE;
					break;
			}
		}
	return FALSE;
}

SquirrelObject SquirrelObject::GetDelegate()
{
	SquirrelObject ret(_VM);
	if(_o._type == OT_TABLE || _o._type == OT_USERDATA)
	{
		sq_pushobject(_VM,_o);
		sq_getdelegate(_VM,-1);
		ret.AttachToStackObject(-1);
		sq_pop(_VM,2);
	}
	return ret;
}

BOOL SquirrelObject::IsNull() const
{
	return sq_isnull(_o);
}

BOOL SquirrelObject::IsNumeric() const
{
	return sq_isnumeric(_o);
}

int SquirrelObject::Len() const
{
	int ret = 0;
	if(sq_isarray(_o) || sq_istable(_o) || sq_isstring(_o)) {
		sq_pushobject(_VM,_o);
		ret = sq_getsize(_VM,-1);
		sq_pop(_VM,1);
	}
	return ret;
}

#define _SETVALUE_INT_BEGIN \
	BOOL ret = FALSE; \
	int top = sq_gettop(_VM); \
	sq_pushobject(_VM,_o); \
	sq_pushinteger(_VM,key);

#define _SETVALUE_INT_END \
	if(SQ_SUCCEEDED(sq_rawset(_VM,-3))) { \
		ret = TRUE; \
	} \
	sq_settop(_VM,top); \
	return ret;

BOOL SquirrelObject::SetValue(INT key,const SquirrelObject &val)
{
	_SETVALUE_INT_BEGIN
	sq_pushobject(_VM,val._o);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(INT key,INT n)
{
	_SETVALUE_INT_BEGIN
	sq_pushinteger(_VM,n);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(INT key,FLOAT f)
{
	_SETVALUE_INT_BEGIN
	sq_pushfloat(_VM,f);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(INT key,const SQChar *s)
{
	_SETVALUE_INT_BEGIN
	sq_pushstring(_VM,s,-1);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(INT key,bool b)
{
	_SETVALUE_INT_BEGIN
	sq_pushbool(_VM,b);
	_SETVALUE_INT_END
}

BOOL SquirrelObject::SetValue(const SquirrelObject &key,const SquirrelObject &val)
{
	BOOL ret = FALSE;
	int top = sq_gettop(_VM);
	sq_pushobject(_VM,_o);
	sq_pushobject(_VM,key._o);
	sq_pushobject(_VM,val._o);
	if(SQ_SUCCEEDED(sq_rawset(_VM,-3))) {
		ret = TRUE;
	}
	sq_settop(_VM,top);
	return ret;
}

#define _SETVALUE_STR_BEGIN \
	BOOL ret = FALSE; \
	int top = sq_gettop(_VM); \
	sq_pushobject(_VM,_o); \
	sq_pushstring(_VM,key,-1);

#define _SETVALUE_STR_END \
	if(SQ_SUCCEEDED(sq_rawset(_VM,-3))) { \
		ret = TRUE; \
	} \
	sq_settop(_VM,top); \
	return ret;

BOOL SquirrelObject::SetValue(const SQChar *key,const SquirrelObject &val)
{
	_SETVALUE_STR_BEGIN
	sq_pushobject(_VM,val._o);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,INT n)
{
	_SETVALUE_STR_BEGIN
	sq_pushinteger(_VM,n);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,FLOAT f)
{
	_SETVALUE_STR_BEGIN
	sq_pushfloat(_VM,f);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,const SQChar *s)
{
	_SETVALUE_STR_BEGIN
	sq_pushstring(_VM,s,-1);
	_SETVALUE_STR_END
}

BOOL SquirrelObject::SetValue(const SQChar *key,bool b)
{
	_SETVALUE_STR_BEGIN
	sq_pushbool(_VM,b);
	_SETVALUE_STR_END
}


SQObjectType SquirrelObject::GetType()
{
	return _o._type;
}

BOOL SquirrelObject::GetSlot(INT key) const
{
	sq_pushobject(_VM,_o);
	sq_pushinteger(_VM,key);
	if(SQ_SUCCEEDED(sq_get(_VM,-2))) {
		return TRUE;
	}
	
	return FALSE;
}


SquirrelObject SquirrelObject::GetValue(INT key)const
{
	SquirrelObject ret(_VM);
	if(GetSlot(key)) {
		ret.AttachToStackObject(-1);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

FLOAT SquirrelObject::GetFloat(INT key) const
{
	FLOAT ret = 0.0f;
	if(GetSlot(key)) {
		sq_getfloat(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

INT SquirrelObject::GetInt(INT key) const
{
	INT ret = 0;
	if(GetSlot(key)) {
		sq_getinteger(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

const SQChar *SquirrelObject::GetString(INT key) const
{
	const SQChar *ret = NULL;
	if(GetSlot(key)) {
		sq_getstring(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

bool SquirrelObject::GetBool(INT key) const
{
	SQBool ret = FALSE;
	if(GetSlot(key)) {
		sq_getbool(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret?true:false;
}

BOOL SquirrelObject::Exists(const SQChar *key) const
{
	BOOL ret = FALSE;
	if(GetSlot(key)) {
		ret = TRUE;
	}
	sq_pop(_VM,1);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL SquirrelObject::GetSlot(const SQChar *name) const
{
	sq_pushobject(_VM,_o);
	sq_pushstring(_VM,name,-1);
	if(SQ_SUCCEEDED(sq_get(_VM,-2))) {
		return TRUE;
	}
	
	return FALSE;
}

SquirrelObject SquirrelObject::GetValue(const SQChar *key)const
{
	SquirrelObject ret(_VM);
	if(GetSlot(key)) {
		ret.AttachToStackObject(-1);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

FLOAT SquirrelObject::GetFloat(const SQChar *key) const
{
	FLOAT ret = 0.0f;
	if(GetSlot(key)) {
		sq_getfloat(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

INT SquirrelObject::GetInt(const SQChar *key) const
{
	INT ret = 0;
	if(GetSlot(key)) {
		sq_getinteger(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

const SQChar *SquirrelObject::GetString(const SQChar *key) const
{
	const SQChar *ret = NULL;
	if(GetSlot(key)) {
		sq_getstring(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret;
}

bool SquirrelObject::GetBool(const SQChar *key) const
{
	SQBool ret = FALSE;
	if(GetSlot(key)) {
		sq_getbool(_VM,-1,&ret);
		sq_pop(_VM,1);
	}
	sq_pop(_VM,1);
	return ret?true:false;
}

SQUserPointer SquirrelObject::GetInstanceUP(SQUserPointer tag) const
{
	SQUserPointer up = NULL;
	sq_pushobject(_VM,_o);
	sq_getinstanceup(_VM,-1,(SQUserPointer*)&up,tag);
	sq_pop(_VM,1);
	return up;
}

BOOL SquirrelObject::SetInstanceUP(SQUserPointer up)
{
	if(!sq_isinstance(_o)) return FALSE;
	sq_pushobject(_VM,_o);
	sq_setinstanceup(_VM,-1,up);
	sq_pop(_VM,1);
	return TRUE;
}

SquirrelObject SquirrelObject::GetAttributes(const SQChar *key)
{
	SquirrelObject ret(_VM);
	int top = sq_gettop(_VM);
	sq_pushobject(_VM,_o);
	if(key)
		sq_pushstring(_VM,key,-1);
	else
		sq_pushnull(_VM);
	if(SQ_SUCCEEDED(sq_getattributes(_VM,-2))) {
		ret.AttachToStackObject(-1);
	}
	sq_settop(_VM,top);
	return ret;
}

BOOL SquirrelObject::BeginIteration()
{
	if(!sq_istable(_o) && !sq_isarray(_o) && !sq_isclass(_o))
		return FALSE;
	sq_pushobject(_VM,_o);
	sq_pushnull(_VM);
	return TRUE;
}

BOOL SquirrelObject::Next(SquirrelObject &key,SquirrelObject &val)
{
	if(SQ_SUCCEEDED(sq_next(_VM,-2))) {
		key.AttachToStackObject(-2);
		val.AttachToStackObject(-1);
		sq_pop(_VM,2);
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
	sq_pop(_VM,2);
}