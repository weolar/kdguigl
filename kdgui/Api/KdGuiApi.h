#ifndef KDGUI3_h
#define KDGUI3_h

#include "KdPageInfo.h"

#ifdef WEBKIT_DLL
#define KDEXPORT /*extern "C"*/ _declspec(dllexport)
#else
#define KDEXPORT 
#endif

#ifndef KDCALL
#define KDCALL WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

typedef struct _KdGuiObj* KdGuiObjPtr;

class PageManagerPublic;

typedef PageManagerPublic* KdPagePtr;
class KdValArray;
class IKdGuiBuffer;

typedef LRESULT (WINAPI* PFN_KdPageWinMsgCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    bool* pbNeedContinue
    );

typedef LRESULT (WINAPI* PFN_KdPageCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd
    );

typedef LRESULT (WINAPI* PFN_KdResCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    const SQChar* pURL,
    IKdGuiBuffer* pAllocate
    );

typedef struct SQVM* HSQUIRRELVM;
typedef LRESULT (WINAPI* PFN_KdPageScriptInitCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    HSQUIRRELVM v
    );

enum KdPagePaintStep {
    KDPPaintStepPrePaintToMemoryDC,
    KDPPaintStepPostPaintToMemoryDC,
    KDPPaintStepPostPaintToScreenDC
};
typedef LRESULT (WINAPI* PFN_KdPagePaintCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    KdPagePaintStep emKdPagePaintStep,
    void** ppCallBackContext,
    bool* pbNeedContinue,
    const RECT* rtPaint,
    HDC hMemoryDC,
    HDC hPaintDC
    );

typedef int (KDCALL* PFN_KdPageError)(void*, const SQChar* , ...);

KDEXPORT KdGuiObjPtr KDCALL KdCreateGuiObj(void* pForeignPtr);

KDEXPORT KdPagePtr KDCALL KdCreateRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle, 
    KdPageInfoPtr pPageInfo,
    void* pForeignPtr,
    bool bShow
    );

KDEXPORT HWND KDCALL KdGetHWNDFromPagePtr(KdPagePtr kdPage);

KDEXPORT void KDCALL
KdCloseRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPagePtr pPage
    );

KDEXPORT bool KDCALL KdLoadPageFormUrl(KdPagePtr kdPageHandle, const SQChar* lpUrl);
KDEXPORT bool KDCALL KdLoadPageFormData(KdPagePtr kdPageHandle, const void* lpData, int nLen);

KDEXPORT void KDCALL KdPostResToAsynchronousLoad(KdPagePtr kdPageHandle, const SQChar* pUrl, void* pResBuf, int nResBufLen, bool bNeedSavaRes);

KDEXPORT void KDCALL KdRegisterXMLOnRealy(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack);
KDEXPORT void KDCALL KdRegisterUninit(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack);
KDEXPORT void KDCALL KdRegisterScriptInit(KdPagePtr kdPageHandle, PFN_KdPageScriptInitCallback pCallBack);
KDEXPORT void KDCALL KdRegisterResHandle(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack);
KDEXPORT void KDCALL KdRegisterResOtherNameQuery(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack);
KDEXPORT void KDCALL KdRegisterPaintCallback(KdPagePtr kdPageHandle, PFN_KdPagePaintCallback pCallBack);
KDEXPORT void KDCALL KdRegisterError2(KdPagePtr kdPageHandle, PFN_KdPageError pCallBack);

KDEXPORT void KDCALL KdRepaintRequested(KdPagePtr kdPageHandle, const RECT* repaintRect);

KDEXPORT void KDCALL KdSendTimerEvent(KdPagePtr kdPageHandle);
KDEXPORT void KDCALL KdSendResizeEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendPaintEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendMouseEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT int  KDCALL KdSendInputEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KDEXPORT void KDCALL KdSendCaptureChanged(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendKillFocusEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KDEXPORT void KDCALL KdRegisterMsgHandle(KdPagePtr kdPageHandle, PFN_KdPageWinMsgCallback pPreCallBack, PFN_KdPageWinMsgCallback pPostCallBack);

KDEXPORT void KDCALL KdInitThread();
KDEXPORT void KDCALL KdUninitThread();

KDEXPORT void KDCALL KdSetBackgroundColor(KdPagePtr kdPageHandle, COLORREF c);

KDEXPORT void KDCALL KdShowDebugNodeData(KdPagePtr kdPageHandle);

KDEXPORT void KDCALL KdCopyMemoryDC(KdPagePtr kdPageHandle, HDC hDC, const RECT* rc);

KDEXPORT BOOL KDCALL KdHasJsonReady();

KDEXPORT void KDCALL KdFireSharedTimer();

KDEXPORT int KDCALL KdSettingChange(KdPagePtr kdPageHandle, const SQChar* settingType, void* setting);
//////////////////////////////////////////////////////////////////////////

typedef void* KVAPtr;

KDEXPORT KVAPtr KDCALL KVACreate(int iElementSize, int iPreallocSize);
KDEXPORT KVAPtr KDCALL KVADelete();
KDEXPORT void KDCALL KVAEmpty(KVAPtr pThis);
KDEXPORT bool KDCALL KVAIsEmpty(KVAPtr pThis);
KDEXPORT bool KDCALL KVAAdd(KVAPtr pThis, LPCVOID pData);
KDEXPORT bool KDCALL KVARemove(KVAPtr pThis, int iIndex);
KDEXPORT bool KDCALL KVAResize(KVAPtr pThis, int iIndex);
KDEXPORT int KDCALL KVAGetSize(KVAPtr pThis);
KDEXPORT void KDCALL KVASetSize(KVAPtr pThis, int nSize);
KDEXPORT LPVOID KDCALL KVAGetData(KVAPtr pThis);
KDEXPORT LPVOID KDCALL KVAGetAt(KVAPtr pThis, int iIndex);

#endif // KDGUI3_h


