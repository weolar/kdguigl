
#include <UIlib.h>
#include <android/log.h>
#include <jni.h>
#include "platform/android/Application.h"
#include "Core/UIManager.h"

extern "C" JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_nativeTouchesBegin(JNIEnv * env, jobject thiz, jint id, jfloat x, jfloat y)
{
	LOGI("nativeTouchesBegin");
    Application::GetInst()->GetPageMgr()->handleTouchesBegin(1, &id, &x, &y);
	LOGI("nativeTouchesBegin ~~");
}

extern "C" JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_nativeTouchesEnd(JNIEnv * env, jobject thiz, jint id, jfloat x, jfloat y)
{
	LOGI("nativeTouchesEnd");
    Application::GetInst()->GetPageMgr()->handleTouchesEnd(1, &id, &x, &y);
	LOGI("nativeTouchesEnd~~");
}

extern "C" JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_nativeTouchesMove(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys) {
    LOGI("nativeTouchesMove");
	int size = env->GetArrayLength(ids);
    jint id[size];
    jfloat x[size];
    jfloat y[size];

    env->GetIntArrayRegion(ids, 0, size, id);
    env->GetFloatArrayRegion(xs, 0, size, x);
    env->GetFloatArrayRegion(ys, 0, size, y);

    Application::GetInst()->GetPageMgr()->handleTouchesMove(size, id, x, y);
	LOGI("nativeTouchesMove~~");
}

extern "C" JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_nativeTouchesCancel(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys) {
    int size = env->GetArrayLength(ids);
    jint id[size];
    jfloat x[size];
    jfloat y[size];

    env->GetIntArrayRegion(ids, 0, size, id);
    env->GetFloatArrayRegion(xs, 0, size, x);
    env->GetFloatArrayRegion(ys, 0, size, y);

    Application::GetInst()->GetPageMgr()->handleTouchesCancel(size, id, x, y);
}

#define KEYCODE_BACK 0x04
#define KEYCODE_MENU 0x52

/*
extern "C" JNIEXPORT jboolean JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_nativeKeyDown(JNIEnv * env, jobject thiz, jint keyCode) {
    CAApplication* pDirector = CAApplication::getApplication();
    switch (keyCode) {
        case KEYCODE_BACK:
              if (pDirector->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked))
                return JNI_TRUE;
            break;
        case KEYCODE_MENU:
            if (pDirector->getKeypadDispatcher()->dispatchKeypadMSG(kTypeMenuClicked))
                return JNI_TRUE;
            break;
        default:
            return JNI_FALSE;
    }
    return JNI_FALSE;
}*/