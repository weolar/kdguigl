# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := kdgui
LOCAL_CFLAGS    := -DSK_RELEASE -D_STLP_USE_SIMPLE_NODE_ALLOC -D_STLP_USE_NEWALLOC -DOS_ANDROID -DWTF_COMPILER_GCC -DNANOVG_GLES2_IMPLEMENTATION#-D_STLP_USE_NO_IOSTREAMS -D_STLP_USE_MALLOC 

APP_CPPFLAGS += -frtti
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -fpermissive

LOCAL_LDLIBS    := -llog -lGLESv2 -landroid

#C:/Users/Administrator/Desktop/adt-bundle-windows-x86-20140321/android-ndk-r8d/sources/cxx-stl/stlport/stlport
#F:/Gui2/eclipse/android-ndk-r8d/sources/cxx-stl/stlport/stlport

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../kdgui\
 $(LOCAL_PATH)/../../../\
$(LOCAL_PATH)/../../../skia/include\
$(LOCAL_PATH)/../../../skia/include/config\
$(LOCAL_PATH)/../../../skia/include/core\
$(LOCAL_PATH)/../../../skia/include/lazy\
$(LOCAL_PATH)/../../../kdgui/script/include\
F:/Gui2/eclipse/android-ndk-r8d/sources/cxx-stl/stlport/stlport

#C:/Users/Administrator/Desktop/adt-bundle-windows-x86-20140321/android-ndk-r8d/sources/cxx-stl/stlport/stlport

LOCAL_SRC_FILES := ../../../kdgui/UILib.cpp \
../../../kdgui/platform/android/Application.cpp\
../../../kdgui/platform/android/Java_org_cocos2dx_lib_Cocos2dxHelper.cpp\
../../../kdgui/platform/android/TouchesJni.cpp\
../../../kdgui/Api/PageManagerPublicAndroid.cpp\
../../../kdgui/bindings/KqPropsPushHelp.cpp\
../../../kdgui/bindings/ScriptApi.cpp\
../../../kdgui/bindings/ScriptDom.cpp\
../../../kdgui/bindings/SqEventListener.cpp\
../../../kdgui/cache/ResCache.cpp\
../../../kdgui/cache/ResSkiaImage.cpp\
../../../kdgui/Core/ScheduledAction.cpp\
../../../kdgui/Core/ScritpMgr.cpp\
../../../kdgui/Core/UIManager.cpp\
../../../kdgui/dom/style/NodeStyle.cpp\
../../../kdgui/dom/ClipPathNode.cpp\
../../../kdgui/dom/GNode.cpp\
../../../kdgui/dom/GradientNodeComm.cpp\
../../../kdgui/dom/ImageNode.cpp\
../../../kdgui/dom/LinearGradientNode.cpp\
../../../kdgui/dom/NodeAnim.cpp\
../../../kdgui/dom/RadialGradientNode.cpp\
../../../kdgui/dom/TextNode.cpp\
../../../kdgui/dom/UINode.cpp\
../../../kdgui/dom/VectorNode.cpp\
../../../kdgui/dom/VectorNodeImpl.cpp\
../../../kdgui/graphics/nanovg/nanovg.cpp\
../../../kdgui/graphics/AffineTransform.cpp\
../../../kdgui/graphics/FloatPoint.cpp\
../../../kdgui/graphics/FloatPoint3D.cpp\
../../../kdgui/graphics/FloatPointSkia.cpp\
../../../kdgui/graphics/FloatRect.cpp\
../../../kdgui/graphics/FloatQuad.cpp\
../../../kdgui/graphics/FloatRectSkia.cpp\
../../../kdgui/graphics/FloatSize.cpp\
../../../kdgui/graphics/FontUtil.cpp\
../../../kdgui/graphics/Gradient.cpp\
../../../kdgui/graphics/GradientSkia.cpp\
../../../kdgui/graphics/GraphicsContext.cpp\
../../../kdgui/graphics/GraphicsContextNanovg.cpp\
../../../kdgui/graphics/IntPointSkia.cpp\
../../../kdgui/graphics/IntPointWin.cpp\
../../../kdgui/graphics/IntRect.cpp\
../../../kdgui/graphics/IntRectSkia.cpp\
../../../kdgui/graphics/IntRectWin.cpp\
../../../kdgui/graphics/IntSizeWin.cpp\
../../../kdgui/graphics/KdColor.cpp\
../../../kdgui/graphics/KdPath.cpp\
../../../kdgui/graphics/PathSkia.cpp\
../../../kdgui/graphics/PlatformContextNanovg.cpp\
../../../kdgui/graphics/RoundedIntRect.cpp\
../../../kdgui/graphics/SkiaUtils.cpp\
../../../kdgui/graphics/TransformationMatrix.cpp\
../../../kdgui/graphics/TransformationMatrixSkia.cpp\
../../../kdgui/rendering/ClipAttrData.cpp\
../../../kdgui/rendering/ClipData.cpp\
../../../kdgui/rendering/RenderSupport.cpp\
../../../kdgui/rendering/ShadowData.cpp\
../../../kdgui/script/scripthelp/SquirrelBindingsUtils.cpp\
../../../kdgui/script/scripthelp/SquirrelObject.cpp\
../../../kdgui/script/scripthelp/SquirrelVM.cpp\
../../../kdgui/script/sqstdlib/sqstdaux.cpp\
../../../kdgui/script/sqstdlib/sqstdblob.cpp\
../../../kdgui/script/sqstdlib/sqstdio.cpp\
../../../kdgui/script/sqstdlib/sqstdmath.cpp\
../../../kdgui/script/sqstdlib/sqstdrex.cpp\
../../../kdgui/script/sqstdlib/sqstdstream.cpp\
../../../kdgui/script/sqstdlib/sqstdstring.cpp\
../../../kdgui/script/sqstdlib/sqstdsystem.cpp\
../../../kdgui/script/squirrel/sqapi.cpp\
../../../kdgui/script/squirrel/sqbaselib.cpp\
../../../kdgui/script/squirrel/sqclass.cpp\
../../../kdgui/script/squirrel/sqcompiler.cpp\
../../../kdgui/script/squirrel/sqdebug.cpp\
../../../kdgui/script/squirrel/SqErrorHandling.cpp\
../../../kdgui/script/squirrel/sqfuncstate.cpp\
../../../kdgui/script/squirrel/sqlexer.cpp\
../../../kdgui/script/squirrel/sqmem.cpp\
../../../kdgui/script/squirrel/sqobject.cpp\
../../../kdgui/script/squirrel/sqstate.cpp\
../../../kdgui/script/squirrel/sqtable.cpp\
../../../kdgui/script/squirrel/sqvm.cpp\
../../../kdgui/svg/SVGParserUtilities.cpp\
../../../kdgui/svg/SVGPathBuilder.cpp\
../../../kdgui/svg/SVGPathParser.cpp\
../../../kdgui/svg/SVGPathParserFactory.cpp\
../../../kdgui/svg/SVGPathStringSource.cpp\
../../../kdgui/wtf/CurrentTime.cpp\
../../../kdgui/wtf/FastMallocAndroid.cpp\
../../../kdgui/wtf/HashTable.cpp\
../../../kdgui/wtf/MainThread.cpp\
../../../kdgui/wtf/platformevent.cpp\
../../../kdgui/wtf/RefCountedLeakCounter.cpp\
../../../kdgui/wtf/StdUtil.cpp\
../../../kdgui/wtf/ThreadSpecificAndroid.cpp\
../../../kdgui/wtf/ThreadTimers.cpp\
../../../kdgui/wtf/Timer.cpp\
../../../kdgui/wtf/UtilHelp.cpp\
../../../skia/src/core/SkString.cpp\
../../../skia/src/core/SkMath.cpp\
../../../skia/src/core/SkMatrix.cpp\
../../../skia/src/core/SkFloat.cpp\
../../../skia/src/core/SkRect.cpp\
../../../skia/src/core/SkRRect.cpp\
../../../skia/src/core/SkScalar.cpp\
../../../skia/src/core/SkPoint.cpp\
../../../skia/src/core/SkRegion.cpp\
../../../skia/src/core/SkRegion_path.cpp\
../../../skia/src/core/SkPath.cpp\
../../../skia/src/core/Sk64.cpp\
../../../skia/src/core/SkGeometry.cpp\
../../../skia/src/core/SkCordic.cpp\
../../../skia/src/core/SkBuffer.cpp\
../../../skia/src/core/SkUtils.cpp\
../../../skia/src/core/SkFixAndroid.cpp\
../../../skia/src/core/SkBlitter.cpp\
stlfix.cpp

include $(BUILD_SHARED_LIBRARY)
