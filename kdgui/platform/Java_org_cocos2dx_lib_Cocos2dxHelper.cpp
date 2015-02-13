#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <android/assets_manager.h>

#include <string>
//#include "JniHelper.h"
#include "Java_org_cocos2dx_lib_Cocos2dxHelper.h"

using namespace std;

extern "C" {

void Java_org_cocos2dx_lib_Cocos2dxActivity_nativeSetAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
{
	gAssetMgr = AAssetManager_fromJava( env, assetManager );
}

}