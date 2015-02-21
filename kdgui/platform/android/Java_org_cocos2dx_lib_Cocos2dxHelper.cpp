#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
//#include "JniHelper.h"
#include "Java_org_cocos2dx_lib_Cocos2dxHelper.h"

extern "C" {

AAssetManager* g_assetMgr = 0;

//void Java_org_cocos2dx_lib_Cocos2dxActivity_nativeSetAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
//void Java_com_kdgui_lib_GL2JNIActivity_nativeSetAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
JNIEXPORT void JNICALL Java_com_kdgui_lib_Cocos2dxRenderer_setAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
{
	g_assetMgr = AAssetManager_fromJava(env, assetManager);
}

}