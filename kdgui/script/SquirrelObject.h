#ifndef _SQUIRREL_OBJECT_H_
#define _SQUIRREL_OBJECT_H_

#include "squirrel.h"

class SquirrelObject
{
	friend class SquirrelVM;
public:
	//void SetVM(HSQUIRRELVM v);
    SquirrelObject(HSQUIRRELVM v);
	virtual ~SquirrelObject();
	SquirrelObject(const SquirrelObject &o);
	SquirrelObject(HSQUIRRELVM v,HSQOBJECT &o);
    SquirrelObject & operator =(const SquirrelObject &o);
	//SquirrelObject & operator =(int n);
	void Append(const SquirrelObject &o);
	void AttachToStackObject(int idx);
	SquirrelObject Clone();
	BOOL SetValue(const SquirrelObject &key,const SquirrelObject &val);
	
	BOOL SetValue(SQInteger key,const SquirrelObject &val);
	BOOL SetValue(INT key,bool b);
	BOOL SetValue(INT key,INT n);
	BOOL SetValue(INT key,FLOAT f);
	BOOL SetValue(INT key,const SQChar *s);

	BOOL SetValue(const SQChar *key,const SquirrelObject &val);
	BOOL SetValue(const SQChar *key,bool b);
	BOOL SetValue(const SQChar *key,INT n);
	BOOL SetValue(const SQChar *key,FLOAT f);
	BOOL SetValue(const SQChar *key,const SQChar *s);
	
	BOOL SetInstanceUP(SQUserPointer up);
	BOOL IsNull() const;
	int IsNumeric() const;
	int Len() const;
	BOOL SetDelegate(SquirrelObject &obj);
	SquirrelObject GetDelegate();
	const SQChar* ToString();
	bool ToBool();
	SQInteger ToInteger();
	SQFloat ToFloat();
	SQUserPointer GetInstanceUP(SQUserPointer tag) const;
	SquirrelObject GetValue(const SQChar *key) const;
	BOOL Exists(const SQChar *key) const;
	FLOAT GetFloat(const SQChar *key) const;
	INT GetInt(const SQChar *key) const;
	const SQChar *GetString(const SQChar *key) const;
	bool GetBool(const SQChar *key) const;
	SquirrelObject GetValue(INT key) const;
	FLOAT GetFloat(INT key) const;
	INT GetInt(INT key) const;
	const SQChar *GetString(INT key) const;
	bool GetBool(INT key) const;
	SquirrelObject GetAttributes(const SQChar *key = NULL);
	SQObjectType GetType();
	HSQOBJECT &GetObject(){return _o;}
	BOOL BeginIteration();
	BOOL Next(SquirrelObject &key,SquirrelObject &value);
	void EndIteration();
    HSQUIRRELVM getScriptVM() {return _VM;}
private:
    //SquirrelObject() {}
	BOOL GetSlot(const SQChar *name) const;
	BOOL GetSlot(INT key) const;
	HSQOBJECT _o;
    HSQUIRRELVM _VM;
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
	SQUserPointer GetInstanceUp(int idx,SQUserPointer tag)
	{
		SQUserPointer self;
		if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&self,tag)))
			return NULL;
		return self;
	}
	SQUserPointer GetUserdata(int idx,SQUserPointer tag)
	{
		SQUserPointer otag;
		SQUserPointer up;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getuserdata(v,idx,&up,&otag))) {
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
    int Return(const SQChar *s, int len)
    {
        sq_pushstring(v,s,len);
        return 1;
    }
	int Return(FLOAT f)
	{
		sq_pushfloat(v,f);
		return 1;
	}
	int Return(INT i)
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

//////////////////////////////////////////////////////////////////////////
// impl
__inline SquirrelObject::SquirrelObject(HSQUIRRELVM v)
{
    _VM = v;
    sq_resetobject(&_o);
}

// SquirrelObject::SetVM(HSQUIRRELVM v)
// {
//     _VM = v;
// }

__inline SquirrelObject::~SquirrelObject()
{
    if(_VM)
        sq_release(_VM,&_o);
}

__inline SquirrelObject::SquirrelObject(const SquirrelObject &o)
{
    _VM = o._VM;
    _o = o._o;
    sq_addref(_VM,&_o);
}

__inline SquirrelObject::SquirrelObject(HSQUIRRELVM v,HSQOBJECT &o)
{
    _VM = v;
    _o = o;
    sq_addref(_VM,&_o);
}

__inline SquirrelObject SquirrelObject::Clone()
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

__inline SquirrelObject & SquirrelObject::operator = (const SquirrelObject &o)
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

__inline void SquirrelObject::Append(const SquirrelObject &o)
{
    if(sq_isarray(_o)) {
        sq_pushobject(_VM,_o);
        sq_pushobject(_VM,o._o);
        sq_arrayappend(_VM,-2);
        sq_pop(_VM,1);
    }
}

__inline void SquirrelObject::AttachToStackObject(int idx)
{
    HSQOBJECT t;
    sq_getstackobj(_VM,idx,&t);
    sq_addref(_VM,&t);
    sq_release(_VM,&_o);
    _o = t;
}

__inline BOOL SquirrelObject::SetDelegate(SquirrelObject &obj)
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

__inline SquirrelObject SquirrelObject::GetDelegate()
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

__inline BOOL SquirrelObject::IsNull() const
{
    return sq_isnull(_o);
}

__inline BOOL SquirrelObject::IsNumeric() const
{
    return sq_isnumeric(_o);
}

__inline int SquirrelObject::Len() const
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

__inline BOOL SquirrelObject::SetValue(INT key,const SquirrelObject &val)
{
    _SETVALUE_INT_BEGIN
        sq_pushobject(_VM,val._o);
    _SETVALUE_INT_END
}

__inline BOOL SquirrelObject::SetValue(INT key,INT n)
{
    _SETVALUE_INT_BEGIN
        sq_pushinteger(_VM,n);
    _SETVALUE_INT_END
}

__inline BOOL SquirrelObject::SetValue(INT key,FLOAT f)
{
    _SETVALUE_INT_BEGIN
        sq_pushfloat(_VM,f);
    _SETVALUE_INT_END
}

__inline BOOL SquirrelObject::SetValue(INT key,const SQChar *s)
{
    _SETVALUE_INT_BEGIN
        sq_pushstring(_VM,s,-1);
    _SETVALUE_INT_END
}

__inline BOOL SquirrelObject::SetValue(INT key,bool b)
{
    _SETVALUE_INT_BEGIN
        sq_pushbool(_VM,b);
    _SETVALUE_INT_END
}

__inline BOOL SquirrelObject::SetValue(const SquirrelObject &key,const SquirrelObject &val)
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

__inline BOOL SquirrelObject::SetValue(const SQChar *key,const SquirrelObject &val)
{
    _SETVALUE_STR_BEGIN
        sq_pushobject(_VM,val._o);
    _SETVALUE_STR_END
}

__inline BOOL SquirrelObject::SetValue(const SQChar *key,INT n)
{
    _SETVALUE_STR_BEGIN
        sq_pushinteger(_VM,n);
    _SETVALUE_STR_END
}

__inline BOOL SquirrelObject::SetValue(const SQChar *key,FLOAT f)
{
    _SETVALUE_STR_BEGIN
        sq_pushfloat(_VM,f);
    _SETVALUE_STR_END
}

__inline BOOL SquirrelObject::SetValue(const SQChar *key,const SQChar *s)
{
    _SETVALUE_STR_BEGIN
        sq_pushstring(_VM,s,-1);
    _SETVALUE_STR_END
}

__inline BOOL SquirrelObject::SetValue(const SQChar *key,bool b)
{
    _SETVALUE_STR_BEGIN
        sq_pushbool(_VM,b);
    _SETVALUE_STR_END
}


__inline SQObjectType SquirrelObject::GetType()
{
    return _o._type;
}

__inline BOOL SquirrelObject::GetSlot(INT key) const
{
    sq_pushobject(_VM,_o);
    sq_pushinteger(_VM,key);
    if(SQ_SUCCEEDED(sq_get(_VM,-2))) {
        return TRUE;
    }

    return FALSE;
}


__inline SquirrelObject SquirrelObject::GetValue(INT key)const
{
    SquirrelObject ret(_VM);
    if(GetSlot(key)) {
        ret.AttachToStackObject(-1);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline FLOAT SquirrelObject::GetFloat(INT key) const
{
    FLOAT ret = 0.0f;
    if(GetSlot(key)) {
        sq_getfloat(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline INT SquirrelObject::GetInt(INT key) const
{
    INT ret = 0;
    if(GetSlot(key)) {
        sq_getinteger(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline const SQChar *SquirrelObject::GetString(INT key) const
{
    const SQChar *ret = NULL;
    if(GetSlot(key)) {
        sq_getstring(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline bool SquirrelObject::GetBool(INT key) const
{
    SQBool ret = FALSE;
    if(GetSlot(key)) {
        sq_getbool(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret?true:false;
}

__inline BOOL SquirrelObject::Exists(const SQChar *key) const
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

__inline BOOL SquirrelObject::GetSlot(const SQChar *name) const
{
    sq_pushobject(_VM,_o);
    sq_pushstring(_VM,name,-1);
    if(SQ_SUCCEEDED(sq_get(_VM,-2))) {
        return TRUE;
    }

    return FALSE;
}

__inline SquirrelObject SquirrelObject::GetValue(const SQChar *key)const
{
    SquirrelObject ret(_VM);
    if(GetSlot(key)) {
        ret.AttachToStackObject(-1);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline FLOAT SquirrelObject::GetFloat(const SQChar *key) const
{
    FLOAT ret = 0.0f;
    if(GetSlot(key)) {
        sq_getfloat(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline INT SquirrelObject::GetInt(const SQChar *key) const
{
    INT ret = 0;
    if(GetSlot(key)) {
        sq_getinteger(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline const SQChar *SquirrelObject::GetString(const SQChar *key) const
{
    const SQChar *ret = NULL;
    if(GetSlot(key)) {
        sq_getstring(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret;
}

__inline bool SquirrelObject::GetBool(const SQChar *key) const
{
    SQBool ret = FALSE;
    if(GetSlot(key)) {
        sq_getbool(_VM,-1,&ret);
        sq_pop(_VM,1);
    }
    sq_pop(_VM,1);
    return ret?true:false;
}

__inline SQUserPointer SquirrelObject::GetInstanceUP(SQUserPointer tag) const
{
    SQUserPointer up = NULL;
    sq_pushobject(_VM,_o);
    sq_getinstanceup(_VM,-1,(SQUserPointer*)&up,tag);
    sq_pop(_VM,1);
    return up;
}

__inline BOOL SquirrelObject::SetInstanceUP(SQUserPointer up)
{
    if(!sq_isinstance(_o)) return FALSE;
    sq_pushobject(_VM,_o);
    sq_setinstanceup(_VM,-1,up);
    sq_pop(_VM,1);
    return TRUE;
}

__inline SquirrelObject SquirrelObject::GetAttributes(const SQChar *key)
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

__inline BOOL SquirrelObject::BeginIteration()
{
    if(!sq_istable(_o) && !sq_isarray(_o) && !sq_isclass(_o))
        return FALSE;
    sq_pushobject(_VM,_o);
    sq_pushnull(_VM);
    return TRUE;
}

__inline BOOL SquirrelObject::Next(SquirrelObject &key,SquirrelObject &val)
{
    if(SQ_SUCCEEDED(sq_next(_VM,-2))) {
        key.AttachToStackObject(-2);
        val.AttachToStackObject(-1);
        sq_pop(_VM,2);
        return TRUE;
    }
    return FALSE;
}

__inline const SQChar* SquirrelObject::ToString()
{
    return sq_objtostring(&_o);
}

__inline SQInteger SquirrelObject::ToInteger()
{
    return sq_objtointeger(&_o);
}

__inline SQFloat SquirrelObject::ToFloat()
{
    return sq_objtofloat(&_o);
}

__inline bool SquirrelObject::ToBool()
{
    //<<FIXME>>
    return _o._unVal.nInteger?true:false;
}

__inline void SquirrelObject::EndIteration()
{
    sq_pop(_VM,2);
}

#endif //_SQUIRREL_OBJECT_H_