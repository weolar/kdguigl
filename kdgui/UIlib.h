
#ifndef KS_UI_LIB_H
#define KS_UI_LIB_H

#ifdef _MSC_VER

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501

#ifdef _DEBUG
#	pragma comment(lib, "lib/skia_d.lib")
#else
#	pragma comment(lib, "lib/skia.lib")
#	include <shlwapi.h>
#endif

#pragma comment(lib, "shlwapi.lib")

#pragma comment(lib, "usp10.lib") // skia_d.lib(SkFontHost_win.obj)”√µΩ
//#pragma comment(lib, "kdgui/InjectTool.lib")

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <algorithm>
#include <atlstr.h>
#include <objbase.h>

#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//using std::max;
//using std::min;

#include <wtf/platform.h>
#include <wtf/platformtype.h>
#include "core/debug.h"

#include <wtf/FastAllocBase.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>
//#include <wtf/CString.h>
#include <wtf/StdString.h>

#include <wtf/StdUtil.h>

#include "graphics/IntSize.h"
#include "graphics/IntPoint.h"
#include "graphics/IntRect.h"
#include "graphics/FloatSize.h"
#include "graphics/FloatRect.h"
#include "graphics/FloatPoint.h"

#include "core/SkDevice.h"
#include "core/SkCanvas.h"
#include "core/SkString.h"
#include "core/SkStream.h"
#include "core/SkTypeface.h"

#include "images/SkImageDecoder.h"
#include "effects/SkGradientShader.h"
#include "Ext/platform_canvas.h"

#include "script/include/squirrel.h"

#endif