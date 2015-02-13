#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <android/assets_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
//#include "JniHelper.h"
#include "Java_org_cocos2dx_lib_Cocos2dxHelper.h"

using namespace std;

extern "C" {

static AAssetManager* g_assetMgr = 0;

void Java_org_cocos2dx_lib_Cocos2dxActivity_nativeSetAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
{
	g_assetMgr = AAssetManager_fromJava(env, assetManager);
}

}