// App.cpp : Defines the entry point for the application.
//

#pragma once

//#include "stdafx.h"
#include "resource.h"
#include "Views.h"

//////////////////////////////////////////////////////////////////////////
//

#define MAX_TOOL_NAME	   30	
#define MAX_TOOL_DESC	   MAX_PATH

//此处的顺序不能改，如作修改，请对代码作相应改动
//cfgp表示路径，cfgf表示文件
enum cfg_path
{
    kis_cfgpRoot = 0,			//	c:\kis\	
    kis_cfgpLng,				//	c:\kis\ressrc\	
    kis_cfgpCurLng,				//	c:\kis\ressrc\chs\	
    kis_cfgpHelp,				//	c:\kis\ressrc\chs\	
    kis_cfgpSnd,				//	c:\kis\ressrc\chs\snd\	
    kis_cfgpWebFile,			//	c:\kis\ressrc\chs\web\	

    kis_cfgpKPFWLog,			//	c:\kis\log\	
    kis_cfgpKPFWDefRules,		//	c:\kis\ressrc\chs\rules\	
    kis_cfgpKPFWUserRules,		//	c:\documents and settings\zqy\application data\kingsoft\kis\kpfw32\rules\	

    kis_cfgpMailDefFilter,		//	c:\kis\ressrc\chs\mf\			
    kis_cfgpMailUserFilter,		//	c:\documents and settings\zqy\application data\kingsoft\kis\mail\mf\	

    kis_cfgpAgentFiles,			//	c:\kis\ressrc\chs\agent\	

    kis_cfgpUplive,				//	c:\kis\update\	自动升级下载的临时目录

    kis_cfgpOEMRoot,			//  c:\kis\oem\0xffffff\

    kis_cfgpCurTheme,			//  c:\kis\ressrc\chs\theme1\

    kis_cfgpEnd					//占位用
};

// OSVERSION枚举值必须跟include\framework\getosversion.h的enumOsPlatform保持一致
enum OSVERSION
{
    kis_getosvfailed,
    kis_osvWindowsUnknonw,
    kis_osvWindows95,
    kis_osvWindows98,
    kis_osvWindowsMe,
    kis_osvWindowsNT4,
    kis_osvWindows2000,
    kis_osvWindowsXP,
    kis_osvWindowsXp64,
    kis_osvWindows2003,
    kis_osvWindows2003R2,
    kis_osvWindowsVista,
    kis_osvWindows7,
    kis_enumOsLonghorn
};

typedef struct _PluginIniInfo
{
    TCHAR _szFileName[MAX_PATH + 1];         // 完整的启动路径+参数
    TCHAR _szToolName[MAX_TOOL_NAME + 1];
    TCHAR _szToolSubName[MAX_TOOL_NAME + 1];
    TCHAR _szToolDesc[MAX_TOOL_DESC + 1];
    TCHAR _szIconNormal[MAX_PATH + 1];
    TCHAR _szIconHover[MAX_PATH + 1];
    TCHAR _szIconBig[MAX_PATH + 1];
}PluginIniInfo;

typedef OSVERSION (__stdcall *kisGetOSVersionFunc)();
typedef int (__stdcall *kisGetPathFunc)(cfg_path, LPWSTR, int);

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef int	(__stdcall *GetUUIDFunc)(char* pszUUID, int* pnLen,	bool bRefresh);

class KKisInfo
{
public:
    KKisInfo();
    BOOL KisInfoInit();
    BOOL GetKisPath(cfg_path pt, LPWSTR Path, int Len) {
        //if (m_pkisGetPath) {return m_pkisGetPath(pt, Path, Len);}
        return FALSE;
    }

private:
    kisGetPathFunc m_pkisGetPath;
    kisGetOSVersionFunc m_pkisGetOSVersion;
};

extern KKisInfo g_KisInfo;
extern std::wstring g_strResPath;

int KReplaceAll(std::wstring& str,  const std::wstring& pattern,  const std::wstring& newpat);

BOOL KHandleSizeForZoom(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Ret);

//////////////////////////////////////////////////////////////////////////

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
   CFrameWindowWnd()  
	   : m_bMaxScreen(FALSE)
	  , m_pm(new CPageManager())
      , m_bHideNor(TRUE) { 
	}

   ~CFrameWindowWnd() {
	   ::DestroyWindow(m_hWnd);
		if (m_pm)
			delete m_pm;
		m_pm = NULL;
   }

   LPCTSTR GetWindowClassName() const { return _T("KdGuiFrame_ByWeolar"); };
   UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME; };

   CPageManager* GetPM() {return m_pm;}

private:
   void OnFinalMessage(HWND /*hWnd*/) { /*delete this;*/ };
   void OnInitDiglag();
   void Notify(TNotifyUI& msg);

   std::wstring GetDialogResource();

   // 从CWindowWnd::__WndProc来
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            OnInitDiglag();
            return 0;
        }
        else if( uMsg == WM_CLOSE  ) {
            delete this;
            ExitProcess(0);
        }
        else if( uMsg == WM_NCHITTEST  ) {
            //LRESULT Ret;
            //if (KHandleSizeForZoom(m_hWnd, uMsg, wParam, lParam, Ret)) {return Ret;}
        }
        //else if( uMsg == WM_NCACTIVATE  ) {return 0;}
        //else if( uMsg == WM_NCCALCSIZE  ) {return 0;}

        LRESULT lRes = 0;
        if( m_pm->MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    CPageManager* m_pm;

    BOOL m_bMaxScreen; // true表示当前是最大化
    BOOL m_bHideNor;   // true表示当前是选中状态，隐藏正常日志
};