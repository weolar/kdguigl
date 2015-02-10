/*
 * Copyright (c) 2010, Kingsoft Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SquirrelBindingsUtils_h
#define SquirrelBindingsUtils_h

#include "squirrel.h"

struct ScriptClassMemberDecl  {
	const SQChar *name;
	SQFUNCTION func;
	int params;
	const SQChar *typemask;
};

struct SquirrelClassDecl  {
	const SQChar *name;
	const SQChar *base;
	const ScriptClassMemberDecl *members;
};

struct ScriptConstantDecl  {
	const SQChar *name;
	SQObjectType type;
	union value {
		value(float v){ f = v; }
		value(int v){ i = v; }
		value(const SQChar *v){ s = v; }
		float f;
		int i;
		const SQChar *s;
	}val;
};

struct ScriptNamespaceDecl  {
	const SQChar *name;
	const ScriptClassMemberDecl *members;
	const ScriptConstantDecl *constants;
	const ScriptClassMemberDecl *delegate;	
};

#define _BEGIN_CLASS(classname)  \
		int SQUIRREL_CALL __##classname##__typeof(HSQUIRRELVM v) \
		{ \
			sq_pushstring(v,_SC(#classname),-1); \
			return 1; \
		} \
		struct ScriptClassMemberDecl __##classname##_members[] = { \
		{_SC("_typeof"),__##classname##__typeof,1,NULL},

#define _BEGIN_NAMESPACE(xnamespace) struct ScriptClassMemberDecl __##xnamespace##_members[] = { 
#define _BEGIN_NAMESPACE_CONSTANTS(xnamespace) {NULL,NULL,NULL,NULL}}; \
	struct ScriptConstantDecl __##xnamespace##_constants[] = { 

#define _BEGIN_DELEGATE(xnamespace) struct ScriptClassMemberDecl __##xnamespace##_delegate[] = { 
#define _DELEGATE(xnamespace) __##xnamespace##_delegate
#define _END_DELEGATE(classname) {NULL,NULL,NULL,NULL}};

#define _CONSTANT(name,type,val) {_SC(#name),type,val},
#define _CONSTANT_IMPL(name,type) {_SC(#name),type,name},

#define _MEMBER_FUNCTION(classname,name,nparams,typemask) \
	{_SC(#name),__##classname##_##name,nparams,typemask},

#define _END_NAMESPACE(classname,delegate) {NULL,OT_NULL,0}}; \
struct ScriptNamespaceDecl __##classname##_decl = {   \
	_SC(#classname), __##classname##_members,__##classname##_constants,delegate };

#define _END_CLASS(classname) {NULL,NULL,NULL,NULL}}; \
struct SquirrelClassDecl __##classname##_decl = {  \
	_SC(#classname), NULL, __##classname##_members }; 
	

#define _END_CLASS_INHERITANCE(classname,base) {NULL,NULL,NULL,NULL}}; \
struct SquirrelClassDecl __##classname##_decl = {  \
	_SC(#classname), _SC(#base), __##classname##_members };

#define _MEMBER_FUNCTION_IMPL(classname,name) \
	SQInteger SQUIRREL_CALL __##classname##_##name(HSQUIRRELVM v)

#define _INIT_STATIC_NAMESPACE(v,classname) SbuCreateStaticNamespace(v,&__##classname##_decl);
#define _INIT_CLASS(v,classname) SbuCreateClass(v,&__##classname##_decl);

#define _DECL_STATIC_NAMESPACE(xnamespace) extern struct ScriptNamespaceDecl __##xnamespace##_decl;
#define _DECL_CLASS(classname) extern struct SquirrelClassDecl __##classname##_decl;

#define _CHECK_SELF(cppclass,scriptclass) \
	cppclass *self = NULL; \
	if(SQ_FAILED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(SQUserPointer)&__##scriptclass##_decl))) { \
		return sq_throwerror(v,_SC("invalid instance type"));\
	}

#define _CHECK_INST_PARAM(pname,idx,cppclass,scriptclass)  \
	cppclass *pname = NULL; \
	if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&pname,(SQUserPointer)&__##scriptclass##_decl))) { \
		return sq_throwerror(v,_SC("invalid instance type"));\
	} \

#define _CHECK_INST_PARAM_BREAK(pname,idx,cppclass,scriptclass)  \
	cppclass *pname = NULL; \
	if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&pname,(SQUserPointer)&__##scriptclass##_decl))) { \
		break; \
	} \

#define _CLASS_TAG(classname) ((unsigned int)&__##classname##_decl)


#define _DECL_NATIVE_CONSTRUCTION(v,classname,cppclass) \
	BOOL push_##classname(v,cppclass &quat); \
	SquirrelObject new_##classname(cppclass &quat);

#define _IMPL_NATIVE_CONSTRUCTION(classname,cppclass) \
static int SQUIRREL_CALL classname##_release_hook(SQUserPointer p, int size) \
{ \
	if(p) { \
		cppclass *pv = (cppclass *)p; \
		delete pv; \
	} \
	return 0; \
} \
BOOL push_##classname(HSQUIRRELVM v,cppclass &quat) \
{ \
	cppclass *newquat = new cppclass; \
	*newquat = quat; \
	if(!SbuCreateNativeClassInstance(v,_SC(#classname),newquat,classname##_release_hook)) { \
		delete newquat; \
		return FALSE; \
	} \
	return TRUE; \
} \
SquirrelObject new_##classname(HSQUIRRELVM v,cppclass &quat) \
{ \
	SquirrelObject ret(v); \
	if(push_##classname(v,quat)) { \
		ret.AttachToStackObject(-1); \
		sq_pop(v, 1); \
	} \
	return ret; \
} \
int construct_##classname(HSQUIRRELVM v,cppclass *p) \
{ \
	sq_setinstanceup(v,1,p); \
	sq_setreleasehook(v,1,classname##_release_hook); \
	return 1; \
}

#define _RETURN_THIS(idx) HSQOBJECT returnThisObj;\
    sq_getstackobj(v, idx, &returnThisObj);\
    ASSERT(OT_INSTANCE == returnThisObj._type);\
    sq_pushobject(v, returnThisObj);\
    return 1;

#define _DECLARE_REFCOUNTED_NEW(v,cppclass,classname) \
	SquirrelObject new_##classname(cppclass *ptr) { \
		if(SbuCreateRefCountedInstance(v,_SC(#classname),ptr)) { \
			HSQOBJECT o; \
			sq_getstackobj(v,-1,&o); \
			SquirrelObject tmp = o; \
			sq_pop(v,1); \
			return tmp; \
		} \
		return SquirrelObject(v) ; \
	}

#define _RETURN_REFCOUNTED_INSTANCE(v,classname,ptr) \
	if(!SbuCreateRefCountedInstance(v,_SC(#classname),ptr)) { \
		return sa.ThrowError(_SC("cannot create the class instance")); \
	} \
	return 1;

#ifdef __cplusplus
extern "C" {
#endif

struct IUnknown;
//SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateStaticClass(HSQUIRRELVM v,SquirrelClassDecl *cd);
SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateStaticNamespace(HSQUIRRELVM v,ScriptNamespaceDecl *sn);
SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateClass(HSQUIRRELVM v,SquirrelClassDecl *cd);
//SQUIRREL_API BOOL SQUIRREL_CALL SbuInitScriptClasses(HSQUIRRELVM v);
SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateNativeClassInstance(HSQUIRRELVM v,const SQChar *classname,SQUserPointer ud,SQRELEASEHOOK hook);
SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateRefCountedInstance(HSQUIRRELVM v,const SQChar *classname,IUnknown *pRC);
//SQUIRREL_API BOOL SQUIRREL_CALL SbuCreateRefCountedInstanceChached(HSQUIRRELVM v,const SQChar *classname,IUnknown *pRC);
SQUIRREL_API SQInteger SQUIRREL_CALL SbuRegisterGlobalFunc(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname);
int SQUIRREL_CALL refcounted_release_hook(SQUserPointer p, int size);
int SQUIRREL_CALL construct_RefCounted(HSQUIRRELVM v, IUnknown *p);

SQUIRREL_API void* SQUIRREL_CALL SbuGetPagePtr(HSQUIRRELVM v);
SQUIRREL_API void* SQUIRREL_CALL SbuGetKdPageHandle(HSQUIRRELVM v);
SQUIRREL_API HWND SQUIRREL_CALL SbuGetHWND(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // SquirrelBindingsUtils_h