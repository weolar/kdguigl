/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SquirrelThread.h"
#include "SquirrelObject.h"
#include "SquirrelPrecompil.h"
#include "Squirrel/squirrel/SqErrorHandling.h"

void SquirrelThread::createThread()
{
    //assert(!m_thread);

    m_thread = sq_newthread(m_parentVm, 1024);  // create m_thread and store it on the stack

    if (0 == m_thread) { 
        ASSERT_NOT_REACHED(); 
        return; 
    }
    
    // Create a HSQOBJECT to hold a reference to the m_thread
    sq_resetobject(&m_threadObj); // init the object

    // store m_thread created by sq_newthread into thread_obj
    if (sq_getstackobj(m_parentVm, -1, &m_threadObj) < 0) { 
        ASSERT_NOT_REACHED(); 
        return; 
    }

    sq_addref(m_parentVm, &m_threadObj); // add reference
    sq_pop(m_parentVm, 1); // remove the m_thread from the stack
}

void SquirrelThread::closeThread()
{
    if (m_thread) {
        //sq_close(m_thread);
        sq_release(m_parentVm, &m_threadObj);
        m_thread = NULL;
    }
}

SquirrelThread::~SquirrelThread()
{
    closeThread();
}

//////////////////////////////////////////////////////////////////////////

void SquirrelThread::initThread(HSQUIRRELVM parentVm)
{
    m_oldTop = -1;
    m_parentVm = parentVm;

    createThread();

    // create a local environment for the m_thread
    HSQOBJECT env;
    sq_resetobject(&env);

    sq_newtable(m_thread);

    // store the object in env
    if(sq_getstackobj(m_thread, -1, &env) < 0) 
    { return; }
    
    sq_addref(m_thread, &env); 
    sq_pop(m_thread, 1); // remove env from stack

    // set old roottable as delegate on env
    sq_pushobject(m_thread, env); // push env
    sq_pushroottable(m_thread);   // [env, root]
    sq_setdelegate(m_thread, -2); // env.set_delegate(root)
    sq_pop(m_thread, 1);          // pop env

    // set env as new roottable
    sq_pushobject(m_thread, env);
    sq_setroottable(m_thread);

    sq_release(m_thread, &env);
}

//////////////////////////////////////////////////////////////////////////
static PBEcode SquirrelPrecompil(WTF::String& code, Vector<UChar>& codeCache) {
    PBEcode	 rt = EnumPBSuccess;
    Vector<UChar> codeFake;

    wcsncpy((wchar_t *)&codeCache[0], (const wchar_t *)code.characters(), code.length());

    int x1 = wcslen((const wchar_t *)&codeCache[0]);

    rt = StrReplaceCase_0((wchar_t *)&codeCache[0], code.length()); // 去掉注释
    if ( rt != EnumPBSuccess )
        goto Exit0;
    
    codeFake = codeCache;
    OutputDebugStringW((wchar_t *)&codeCache[0]);

    int xx = wcslen((const wchar_t *)&codeFake[0]);
    

    rt = StrReplaceCase_1((const wchar_t *)codeFake.data(), code.length(), (wchar_t *)codeCache.data(), codeCache.size()); // 识别class
    if ( rt != EnumPBSuccess )
        goto Exit0;

    OutputDebugStringW((wchar_t *)&codeCache[0]);

    rt = StrReplaceCase_2((wchar_t *)&codeCache[0], codeCache.size()); // 识别.prototype.
    if ( rt != EnumPBSuccess )
        goto Exit0;

    OutputDebugStringW((wchar_t *)&codeCache[0]);

    rt = StrReplaceCase_3((wchar_t *)&codeCache[0], codeCache.size()); // 去掉new 
    if ( rt != EnumPBSuccess )
        goto Exit0;

    rt = StrReplaceCase_4((wchar_t *)&codeCache[0], codeCache.size()); // for in
    if ( rt != EnumPBSuccess )
        goto Exit0;

    OutputDebugStringW((wchar_t *)&codeCache[0]);

Exit0:
    return rt;
}

void raiseExceptionWhenLoadScript(HSQUIRRELVM v, const String& codeName, int errorResult)
{
    g_sq_error_result = errorResult;
    char* recordParams = (char*)malloc(1001);
    memset(recordParams, 0, 1001);

    String codeNameTemp = codeName;

    SQPRINTFUNCTION pf = sq_geterrorfunc(v);
    if(pf)
        pf(v, L"*load script error*: %d %s", g_sq_error_result, codeNameTemp.charactersWithNullTermination());

    if (codeName.length() > 12)
        codeNameTemp = codeName.right(15);
    
    sprintf_s(recordParams, 1000, "%d %s", g_sq_error_result, codeNameTemp.utf8().data());

    OutputDebugStringA(recordParams);
    ::RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, 15, (ULONG_PTR *)recordParams);
    free(recordParams);
    DebugBreak();
}

bool SquirrelThread::loadScript(const String& code, const String& codeName)
{		
    m_oldTop = sq_gettop(m_thread);

    if (code.length() > 7 && 0 == wcsncmp((const wchar_t *)code.characters(), L"//#skip", 7))
        return true;
#if 0
    //int codeLen = wcslen(sCode);
    int codeLen = code.length();
    if (m_codeCache0.size() < (int)codeLen) {
        m_codeCache0.resize(codeLen * 2);
    }

    memset(&m_codeCache0[0], 0, m_codeCache0.size()*sizeof(UChar));

    if (EnumPBSuccess != SquirrelPrecompil(code, m_codeCache0)) {
        notImplemented();
        return false;
    }
    
    OutputDebugStringW((PWCHAR)m_codeCache0.data());

    const SQChar* codeNameString = codeName.charactersWithNullTermination();
    SQRESULT hr = sq_compilebuffer(m_thread, (const SQChar *)m_codeCache0.data(), 
        (int)wcslen((const wchar_t *)m_codeCache0.data()), codeNameString, SQTrue);
    if (SQ_FAILED(hr)) { 
        notImplemented();
        return false;
    }
#endif

#if 1
    String codeNameDummy = codeName;
    const SQChar* codeNameString = codeNameDummy.charactersWithNullTermination();
    SQRESULT hr = sq_compilebuffer(m_thread, (const SQChar *)code.characters(),
        code.length(), codeNameString, SQTrue);
    if (SQ_FAILED(hr)) {
        //raiseExceptionWhenLoadScript(m_thread, codeName, 14);
        //notImplemented();
        return false;
    }
#endif
    // start the script that was previously compiled
    sq_pushroottable(m_thread);
    if (SQ_FAILED(sq_call(m_thread, 1, SQFalse, SQTrue))) {
// 		if (isThrowSqException()) {
// 			raiseExceptionWhenLoadScript(m_thread, codeName, 15);
// 			notImplemented();
// 		}
        sq_pop(m_thread, 1); // pop the compiled closure
        return false;
    } else {
        if (sq_getvmstate(m_thread) != SQ_VMSTATE_IDLE) { 
            //raiseExceptionWhenLoadScript(m_thread, codeName, 16);
            //notImplemented();
            return false; 
        }

        sq_pop(m_thread, 1); // pop the compiled closure
    }
    
    return true;
}

SquirrelObject* SquirrelThread::compileBuffer(const SQChar *code, const SQChar *codeName)
{
    notImplemented();
    return 0;
//     if(!SUCCEEDED(sq_compilebuffer(m_thread, sCode, 
//         (int)wcslen(sCode)*sizeof(SQChar), _SC("console buffer"), 1))) {
//         return 0;
//     }
//     SquirrelObject* ret = new SquirrelObject(m_thread);
//     ret->AttachToStackObject(-1);
//     sq_pop(m_thread, 1);
//     return ret;
}

bool SquirrelThread::runScriptByFunctionName(const SQChar * sFunction)
{		
    m_oldTop = sq_gettop(m_thread);

    // Lookup the function in the roottable and put it on the stack
    sq_pushroottable(m_thread);
    sq_pushstring(m_thread, sFunction, -1);
    if (SQ_SUCCEEDED(sq_get(m_thread, -2))) {
        sq_pushroottable(m_thread);

        if (SQ_FAILED(sq_call(m_thread, 1, SQFalse, SQTrue))) {
            sq_settop(m_thread, m_oldTop);
            return false;
        } else {
            if(sq_getvmstate(m_thread) != SQ_VMSTATE_SUSPENDED) 
            { sq_settop(m_thread, m_oldTop); }
        }
    } else {
        sq_settop(m_thread, m_oldTop);
    }

    return true;
}

bool SquirrelThread::runScript()
{
    notImplemented();
//     sq_pushroottable(m_thread);
//     
//     if (SQ_FAILED(sq_call(m_thread,1,1,1)))
//     { return false; }
    
    return true;
}

bool SquirrelThread::setForeignPtr(void* foreignPtr)
{
    if (!m_thread)
    { return false; }
    
    sq_setforeignptr(m_thread, foreignPtr);

    return true;
}
