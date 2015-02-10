/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#include <stdarg.h>
#include "sqvm.h"
#include "sqfuncproto.h"
#include "sqclosure.h"
#include "sqstring.h"
#include "SqErrorHandling.h"

SQRESULT sq_getfunctioninfo(HSQUIRRELVM v,SQInteger level,SQFunctionInfo *fi)
{
	SQInteger cssize = v->_callsstacksize;
	if (cssize > level) {
		SQVM::CallInfo &ci = v->_callsstack[cssize-level-1];
		if(sq_isclosure(ci._closure)) {
			SQClosure *c = _closure(ci._closure);
			SQFunctionProto *proto = c->_function;
			fi->funcid = proto;
			fi->name = type(proto->_name) == OT_STRING?_stringval(proto->_name):_SC("unknown");
			fi->source = type(proto->_name) == OT_STRING?_stringval(proto->_sourcename):_SC("unknown");
			return SQ_OK;
		}
	}
    g_sq_error_result = 1;
	return sq_throwerror(v,_SC("the object is not a closure"));
}

SQRESULT sq_stackinfos(HSQUIRRELVM v, SQInteger level, SQStackInfos *si)
{
	SQInteger cssize = v->_callsstacksize;
	if (cssize > level) {
		memset(si, 0, sizeof(SQStackInfos));
		SQVM::CallInfo &ci = v->_callsstack[cssize-level-1];
		switch (type(ci._closure)) {
		case OT_CLOSURE:{
			SQFunctionProto *func = _closure(ci._closure)->_function;
			if (type(func->_name) == OT_STRING)
				si->funcname = _stringval(func->_name);
			if (type(func->_sourcename) == OT_STRING)
				si->source = _stringval(func->_sourcename);
			si->line = func->GetLine(ci._ip);
						}
			break;
		case OT_NATIVECLOSURE:
			si->source = _SC("NATIVE");
			si->funcname = _SC("unknown");
			if(type(_nativeclosure(ci._closure)->_name) == OT_STRING)
				si->funcname = _stringval(_nativeclosure(ci._closure)->_name);
			si->line = -1;
			break;
		default: break; //shutup compiler
		}
		return SQ_OK;
    } else {
        switch (type(v->temp_reg)) {
        case OT_CLOSURE:{
            memset(si, 0, sizeof(SQStackInfos));
            si->source = _SC("NATIVE");
            si->funcname = _SC("unknown");
            SQFunctionProto *func = _closure(v->temp_reg)->_function;
            if (type(func->_name) == OT_STRING)
                si->funcname = _stringval(func->_name);
            if (type(func->_sourcename) == OT_STRING)
                si->source = _stringval(func->_sourcename);
            si->line = -1;
            return -2;
                        }
            break;
        }
    }
	return SQ_ERROR;
}

#pragma warning(push)  
#pragma warning(disable : 4996) 

void SQVM::Raise_Error(const SQChar *s, ...)
{
	va_list vl;
	va_start(vl, s);

	scvsprintf(_sp(rsl((SQInteger)scstrlen(s)+(NUMBER_MAX_CHAR*2))), s, vl);

	va_end(vl);

    const SQChar * err = _spval;
	_lasterror = SQString::Create(_ss(this),_spval,-1);
}

void SQVM::Raise_Error(const SQObjectPtr &desc)
{
	_lasterror = desc;
}

SQString *SQVM::PrintObjVal(const SQObjectPtr &o)
{
	switch(type(o)) {
	case OT_STRING: return _string(o);
	case OT_INTEGER:
		scsprintf(_sp(rsl(NUMBER_MAX_CHAR+1)), _PRINT_INT_FMT, _integer(o));
		return SQString::Create(_ss(this), _spval);
		break;
	case OT_FLOAT:
		scsprintf(_sp(rsl(NUMBER_MAX_CHAR+1)), _SC("%.14g"), _float(o));
		return SQString::Create(_ss(this), _spval);
		break;
	default:
		return SQString::Create(_ss(this), GetTypeName(o));
	}
}

#pragma warning(pop)

void SQVM::Raise_IdxError(const SQObjectPtr &o)
{
	SQObjectPtr oval = PrintObjVal(o);
    g_sq_error_result = 2;
	Raise_Error(_SC("the index '%.50s' does not exist"), _stringval(oval));
}

void SQVM::Raise_CompareError(const SQObject &o1, const SQObject &o2)
{
	SQObjectPtr oval1 = PrintObjVal(o1), oval2 = PrintObjVal(o2);
    g_sq_error_result = 3;
	Raise_Error(_SC("comparsion between '%.50s' and '%.50s'"), _stringval(oval1), _stringval(oval2));
}


void SQVM::Raise_ParamTypeError(SQInteger nparam,SQInteger typemask,SQInteger type)
{
	SQObjectPtr exptypes = SQString::Create(_ss(this), _SC(""), -1);
	SQInteger found = 0;	
	for(SQInteger i=0; i<16; i++)
	{
		SQInteger mask = 0x00000001 << i;
		if(typemask & (mask)) {
			if(found>0) StringCat(exptypes,SQString::Create(_ss(this), _SC("|"), -1), exptypes);
			found ++;
			StringCat(exptypes,SQString::Create(_ss(this), IdType2Name((SQObjectType)mask), -1), exptypes);
		}
	}
    g_sq_error_result = 4;
	Raise_Error(_SC("parameter %d has an invalid type '%s' ; expected: '%s'"), nparam, IdType2Name((SQObjectType)type), _stringval(exptypes));
}
