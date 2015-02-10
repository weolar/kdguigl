
#include <UIlib.h>

#ifdef _MSC_VER

using std::min;
using std::max;

#include <gdiplus.h>

#endif // _MSC_VER

#include <wtf/UtilHelp.h>

#include "MainThread.h"
#include "dom/style/NodeStyle.h"
#include "cache/ResCache.h"
#include "ThreadSpecific.h"
#include "graphics/SkiaUtils.h"

static ULONG_PTR g_nGdiplusToken = 0;

void MTInit() {
	WTF::ThreadSpecificInitTls();
	NodeStyle::FindStyleIdByName(0);
	ResCache::GetCache();
	UHEventNameToEnum(NULL);

#ifdef _MSC_VER
	if (!g_nGdiplusToken) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&g_nGdiplusToken, &gdiplusStartupInput,NULL);
	}
#endif // _MSC_VER
}

void MTUninit() {
#ifdef _MSC_VER
	DestroyScratchContext();
	WTF::ThreadSpecificThreadExit();
#endif // _MSC_VER
}
