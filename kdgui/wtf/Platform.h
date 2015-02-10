#ifndef WTF_Platform_h
#define WTF_Platform_h

#define COMPILER(WTF_FEATURE) (defined WTF_COMPILER_##WTF_FEATURE  && WTF_COMPILER_##WTF_FEATURE)
#define ENABLE(WTF_FEATURE) (defined ENABLE_##WTF_FEATURE && ENABLE_##WTF_FEATURE)
#define HAVE(WTF_FEATURE) (defined HAVE_##WTF_FEATURE  && HAVE_##WTF_FEATURE)
#define CPU(WTF_FEATURE) (defined WTF_CPU_##WTF_FEATURE  && WTF_CPU_##WTF_FEATURE)
#define OS(WTF_FEATURE) (defined WTF_OS_##WTF_FEATURE  && WTF_OS_##WTF_FEATURE)
#define USE(WTF_FEATURE) (defined WTF_USE_##WTF_FEATURE  && WTF_USE_##WTF_FEATURE)

//////////////////////////////////////////////////////////////////////////

#ifdef OS_ANDROID

#define ENABLE_WTF_MALLOC_VALIDATION 1
#define HAVE_STDINT_H 1
#define WTF_COMPILER_MSVC 0
#define WTF_OS_WINDOWS 0

#define WTF_CHANGES 1
#define HAVE_VIRTUALALLOC 1

//////////////////////////////////////////////////////////////////////////

#define  LOG_TAG    "JNIMsg"
#define  LOGI(...)  do {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);} while(0);

#define STDMETHODCALLTYPE 
#define TRUE 1
#define FALSE 0

#define __in 
#define __out 
#define __out_opt 
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

//////////////////////////////////////////////////////////////////////////

#elif _MSC_VER

#define ENABLE_WTF_MALLOC_VALIDATION 1
#define HAVE_STDINT_H 1
#define WTF_CPU_X86 1
#define WTF_COMPILER_MSVC 1
#define WTF_OS_WINDOWS 1

#define WTF_CHANGES 1
#define HAVE_VIRTUALALLOC 1

#endif

#define reinterpret_cast_ptr reinterpret_cast

#endif // WTF_Platform_h