// App.cpp : Defines the entry point for the application.
//

//#include "stdafx.h"
#include "resource.h"
#include <string>
#include <shellapi.h>
#include <Shlwapi.h>
#include <atltime.h>
#include <commdlg.h>

#include "dom/UINode.h"
#include "Views.h"
#include "app.h"
#include "wtf/MainThread.h"

#define defWinUniqueMutex	_T("{E9C76B02-5038-4bd8-BAC5-9F3903108E69}")
#define defWinTitle			_T("金山毒霸日志管理器")

//////////////////////////////////////////////////////////////////////////
void CFrameWindowWnd::OnInitDiglag()
{
	//CDialogBuilder builder;

	this->SetIcon(IDR_MAINFRAME);
	m_pm->Init(m_hWnd);
	//UINode* pRoot = builder.Create(&m_pm, GetDialogResource().c_str());
	// 	UINode* pRoot = NULL;
	// 	if (m_pDefaultRes) {
	// 		pRoot = builder.CreateFromNode(&m_pm, m_pDefaultRes, NULL);
	// 	} else {
	// 		pRoot = builder.CreateFromFile(&m_pm, L"F:\\KxE\\kis_2012_ui_fb\\tools\\KdGui9801\\res\\dlg.xml", NULL);
	// 	}
	// 
	// 	ASSERT(pRoot && L"Failed to parse XML");
	m_pm->LoadSyncScriptFromSrc(L"F:\\KxE\\kis_2012_ui_fb\\tools\\KdGui9801\\res\\dlg.svg");

	//m_pm.AttachDialog();
	m_pm->AddNotifier(this);
	m_pm->GetRoot()->Init();

	//CenterWindow();

	return;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MTInit();
	CPageManager::SetResourceInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if(FAILED(Hr))
		return 0;

	int x = 200;
	int y = 200;

	CFrameWindowWnd* pFrame = new CFrameWindowWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, defWinTitle, WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX, 0, 
		x, y, 440, 330, 0);
	CPageManager::MessageLoop();
	delete pFrame;

	::CoUninitialize();
	MTUninit();
	return 0;
}

BOOL APIENTRY DllMain111( 
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CFrameWindowWnd::Notify(TNotifyUI& msg)
{
//     if( msg.sType == _T("NcLButtonDown") ) {
//         int x = ((POINT*)(msg.wParam))->x;
//         int y = ((POINT*)(msg.wParam))->y;
//         ::PostMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(x, y));
//     }else if( msg.sType == _T("NcLButtonDClick") ) {
//         OnMaxMinBtn();
//     }else if( msg.sType == _T("itemselect") ) { // 切换页面
//         CTabFolderUI* pTab = (CTabFolderUI*)msg.pSender;
//         UINode* pCtrlBtn =  m_pm.FindControl(L"HideNorBtn");
//         UINode* pCtrlTxt =  m_pm.FindControl(L"HideNorTxt");
//         if (0 == pTab->GetCurSel()) {
//             if (pCtrlBtn) {pCtrlBtn->SetVisible(TRUE);}
//             if (pCtrlTxt) {pCtrlTxt->SetVisible(TRUE);}
//         }else{
//             if (pCtrlBtn) {pCtrlBtn->SetVisible(FALSE);}
//             if (pCtrlTxt) {pCtrlTxt->SetVisible(FALSE);}
//         }
//     }else if( msg.sType == _T("link") ) { // 查看详情
//         if (NULL == msg.pSender->GetUesrData()) {return;}
// 
//         KLogBase* pLogBase = (KLogBase*)msg.pSender->GetUesrData();
//         if (emLogType_Uplive != pLogBase->nType)
//         {
//             CCheckDetalWnd* pDetal = new CCheckDetalWnd;
//             pDetal->SetLogBase(pLogBase);
//             pDetal->Create(m_hWnd, L"查看日志详情", WS_POPUP|WS_VISIBLE, 0, 230, 240, 674, 478, 0);
//             pDetal->ShowModal();
//             delete pDetal;
//         } else {
//             CCheckUpdataDetalWnd* pDetal = new CCheckUpdataDetalWnd;
//             pDetal->SetLogBase((KUpliveLog*)pLogBase);
//             pDetal->SetListMain((CVerticalLayoutUI*)msg.pSender->GetParent()->GetParent()->GetParent());
//             pDetal->SetFrameWindow(this);
//             pDetal->Create(m_hWnd, L"查看升级日志详情", WS_POPUP|WS_VISIBLE, 0, 230, 240, 674, 478, 0);
//             pDetal->ShowModal();
//             delete pDetal;
//         }
//     }else if( msg.sType == _T("click") ) {
//         if (msg.pSender->GetName() == L"CloseBtn" ||
//             msg.pSender->GetName() == L"CloseBigBtn") { // 关闭
//             m_vecKillVirLog.clear();
//             m_vecUpdataLog.clear();
//             Close();
//         }else if (msg.pSender->GetName() == L"FlushAllBtn"){ // 刷新
//             OnFlushAll();
//         }else if (msg.pSender->GetName() == L"HideNorBtn" ||
//                   msg.pSender->GetName() == L"HideNorTxt") { // 隐藏正常
//             OnHideNorClick();
//         }else if (msg.pSender->GetName() == L"ClearBtn") { // 清空
// //             CListUI* pListCtrl = NULL;
// //             CTabFolderUI* pTabCtrl = (CTabFolderUI*)m_pm.FindControl(L"TabFolder1");
// //             if (NULL == pTabCtrl) {return;}
// //             if (pTabCtrl->GetCurSel() == 0) {
// //                 pListCtrl = (CListUI*)m_pm.FindControl(L"KillVirList");
// //                 if (NULL == pListCtrl) {return;}
// // 
// //                 pListCtrl->RemoveAll();
// //             } else {
// //                 pListCtrl = (CListUI*)m_pm.FindControl(L"UpdataList");
// //                 if (NULL == pListCtrl) {return;}
// // 
// //                 pListCtrl->RemoveAll();
// //             }
//             if (IDNO == ::KMsgBoxWnd(m_hWnd, L"确认清空病毒查杀和产品升级的所有日志吗？", L"清空日志")) {return;}
// 
//             CListUI* pListCtrl = NULL;
//             pListCtrl = (CListUI*)m_pm.FindControl(L"KillVirList");
//             if (pListCtrl) {pListCtrl->RemoveAll(); m_pm.EmptyNameHash();}
//             m_vecKillVirLog.clear();
//             
//             pListCtrl = (CListUI*)m_pm.FindControl(L"UpdataList");
//             if (pListCtrl) {pListCtrl->RemoveAll(); m_pm.EmptyNameHash();}
//             m_vecUpdataLog.clear();
// 
//             m_logAnalyzer.ClearLog();
//             m_logAnalyzer.Reset();
//         }else if (msg.pSender->GetName() == L"ExportBtn") { // 导出
//             OnExportBtn();
//         }else if (msg.pSender->GetName() == L"MiNiBtn") { // 最小化
//             ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
//         }else if (msg.pSender->GetName() == L"MaxBtn2") { // 最大化
//             OnMaxMinBtn();
//         }else if (msg.pSender->GetName() == L"MaxBtn1") { // 最大化还原
//             OnMaxMinBtn();
//         }else if ( wcsncmp(msg.pSender->GetName(), L"List_Vir", 8)  == 0) { // 高亮
//             OnHightLightVirList((CCommPanelUI*)msg.pSender->GetUesrData());
//         }else if ( wcsncmp(msg.pSender->GetName(), L"List_Upd", 8)  == 0) { 
//             OnHightLightUpdList((CCommPanelUI*)msg.pSender->GetUesrData());
//         }else if ( wcsncmp(msg.pSender->GetName(), L"List_Click_Vir", 14)  == 0) { 
//             OnHightLightVirList((CCommPanelUI*)msg.pSender->GetUesrData2());
//         }else if ( wcsncmp(msg.pSender->GetName(), L"List_Click_Upd", 14)  == 0) { 
//             OnHightLightUpdList((CCommPanelUI*)msg.pSender->GetUesrData2());
//         }
//     }else if ( msg.sType == _T("rclick") ){
// 
//     }

    return;
}

std::wstring CFrameWindowWnd::GetDialogResource()
{
    std::wstring strRet = L"<Dialog>"
         L"<DiyLayout name=\"weolar\" scrollbar=\"false\">"
  
         L"<DlgClientArea name=\"ClientArea\" zrol=\"3\" ipos=\"0, 0, 572, 40\" stretch=\"movesize_x\" />"

         L"<Button name=\"M1iNiBtn\" fadeout=\"10\" zrol=\"1\" ipos=\"580,0,24,17\" stretch=\"move_x\"\
          nor_bd=\"%images_res%main_mini_normal.png\" \
          pre_bd=\"%images_res%main_mini_down.png\" \
          ove_bd=\"%images_res%main_mini_hover.png\" />"

         L"<Button name=\"MaxBtn2\" zrol=\"1\" fadeout=\"10\" ipos=\"604,0,24,17\" stretch=\"move_x\"\
           nor_bd=\"%images_res%qianpin_02_normal.png\" \
           pre_bd=\"%images_res%qianpin_02_down.png\" \
           ove_bd=\"%images_res%qianpin_02_hover.png\" />"

         L"<Button name=\"MaxBtn1\" zrol=\"1\" fadeout=\"10\" ipos=\"604,0,24,17\" stretch=\"move_x\" visible=\"false\"\
           nor_bd=\"%images_res%qianpin_01_normal.png\" \
           pre_bd=\"%images_res%qianpin_01_down.png\" \
           ove_bd=\"%images_res%qianpin_01_hover.png\" />"

         L"<Button name=\"CloseBtn\" zrol=\"1\" fadeout=\"10\" ipos=\"628,0,31,17\" stretch=\"move_x\"\
          nor_bd=\"%images_res%main_close_normal.png\" \
          pre_bd=\"%images_res%main_close_down.png\" \
          ove_bd=\"%images_res%main_close_hover.png\" />"

         //L"<MsgResponse ipos=\"145,115,390,328\" zrol=\"6\"/>"

         // 背景等
         L"<ImagePanel name=\"riz_06_small_png\" ipos=\"5,5,17,17\" image=\"%images_res%riz_06_samll.png\" zrol=\"2\" />"
         L"<TextPanel text=\"<b><c #c06e0f>金山毒霸日志管理器</c></b>\" ipos=\"26,6,160,18\"  zrol=\"2\"/>"

         L"<FastImagePanel name=\"background\" stretch=\"movesize_xy\" ipos=\"0,0,674,478\" image=\"%images_res%bj_03.png\" />"
         L"<FastImagePanel name=\"backframe\" ipos=\"8,25,653,407\" image=\"%images_res%bk.png\" stretch=\"movesize_xy\" />"

         L"<FastImagePanel name=\"riz_06_big_png\" ipos=\"50,50,52,46\" image=\"%images_res%riz_06.png\" zrol=\"2\" />"
         L"<TextPanel text=\"<b><c #c06e0f>您可以在此查看病毒查杀、防御监控、升级相关的历史记录及其详细信息</c></b>\"    ipos=\"117,62,440,20\"  zrol=\"2\" />"

         L"<CommPanel pos=\"11,29,658,146\" stretch=\"movesize_x\"  main_color=\"d5b482\" frame_color=\"fffaf3,fffffff,v\" frame_style=\"PS_NULL\" frame_thick=\"0\" />"
         L"<CommPanel ipos=\"12,146,644,1\" stretch=\"movesize_x\"  main_color=\"d5b482\" frame_color=\"c06e0f,c06e0f,v\" frame_style=\"PS_SOLID\" frame_thick=\"1\" />"
         L"<TabFolder name=\"TabFolder1\" stretch=\"movesize_xy\" client_transp=\"true\" tab_space=\"18\"\
          nor_bd=\"%images_res%riz_12.png\" \
          ove_bd=\"%images_res%riz_12.png\" \
          pre_bd=\"%images_res%riz_10.png\" \
          ipos=\"12,110,646,317\" \
          nor_h=\"23\" \
          ove_h=\"23\" \
          pre_h=\"23\" \
          nor_w=\"100\" \
          ove_w=\"100\" \
          pre_w=\"100\" \
          tab_first_space=\"15\" >"
          L"<TabPage name=\"TabPage1\" text=\"病毒查杀\" >"

          L"<DiyLayout scrollbar=\"false\" name=\"DiyLayoutList__1\">"

          L"<List name=\"KillVirList\" xstretch=\"true\" stretch=\"movesize_xy\" ipos=\"0,8,645,286\" zrol=\"1\" footer=\"hidden\" >"
          L"<HeadBdEleDiv name=\"HeadBdDiv\" width=\"5000\">"
          L"<CommPanel ipos=\"0,0,5650,22\"  frame_color=\"f9f3e8,fffffff,v\" frame_style=\"PS_NULL\" frame_thick=\"0\" />"
          L"</HeadBdEleDiv>"

          L"<ListHeaderItem  width=\"143\" height=\"30\" name=\"ListHeaderItem__1\">"
          L"<TextPanel ipos=\"0,0,140,20\" text=\"<c #887b5B>时间</c>\" stretch=\"movesize_x\" name=\"TextPanel__1\" textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"75\" height=\"30\" name=\"ListHeaderItem__2\">"
          L"<TextPanel pos=\"5,0,73,20\" text=\"<c #887b5a>事件</c>\" name=\"TextPanel__2\" stretch=\"movesize_x\" name=\"TextPanel__1\"textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"300\" height=\"30\" name=\"ListHeaderItem__3\">"
          L"<TextPanel pos=\"0,0,300, 20\" text=\"<c #887b5a>查杀结果</c>\" name=\"TextPanel__3\" stretch=\"movesize_x\" name=\"TextPanel__1\"textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"100\" height=\"30\" name=\"ListHeaderItem__4\">"
          L"<TextPanel pos=\"30,0,60,20\" text=\"<c #887b5a>详情</c>\" name=\"TextPanel__4\" />"
          L"</ListHeaderItem>"

          L"</List>"

          L"</DiyLayout>"

          L"</TabPage>"

          //////////////////////////////////////////////////////////////////////////
          L"<TabPage name=\"TabPage2\" text=\" 产品升级 \" >"// 
 
          L"<DiyLayout scrollbar=\"false\" name=\"DiyLayoutList__2\" >"

          L"<List name=\"UpdataList\" xstretch=\"true\" stretch=\"movesize_xy\" ipos=\"0,8,645,286\" zrol=\"1\" footer=\"hidden\" >"
          L"<HeadBdEleDiv name=\"HeadBdDiv\" width=\"5000\">"
          L"<CommPanel ipos=\"0,0,5650,22\"  frame_color=\"f9f3e8,fffffff,v\" frame_style=\"PS_NULL\" frame_thick=\"0\" />"
          L"</HeadBdEleDiv>"

          L"<ListHeaderItem  width=\"180\" height=\"30\">"
          L"<TextPanel pos=\"0,0,180, 20\" text=\"<c #887b5a>时间</c>\" stretch=\"movesize_x\" textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\" name=\"TextPanel_2\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"154\" height=\"30\">"
          L"<TextPanel pos=\"0,0,154, 20\" text=\"<c #887b5a>事件</c>\" stretch=\"movesize_x\" textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"140\" height=\"30\">"
          L"<TextPanel pos=\"0,0,140, 20\" text=\"<c #887b5a>结果</c>\" stretch=\"movesize_x\" textStyle=\"DT_SINGLELINE\" textStyle=\"DT_CENTER\"/>"
          L"</ListHeaderItem>"

          L"<ListHeaderItem  width=\"90\" height=\"30\">"
          L"<TextPanel pos=\"30,0,60,20\" text=\"<c #887b5a>详情</c>\" />"
          L"</ListHeaderItem>"

          L"</List>"

          L"</DiyLayout>"
 
          L"</TabPage>"
 
          L"</TabFolder>"

          // CheckBox
          L"<PolyImage ipos=\"17,446,15,15\" list_arr=\"30|31|\" first_index=\"0\" name=\"HideNorBtn\" stretch=\"move_y\"\
          image_list=\"%images_res%check.png|%images_res%nocheck.png|\" />"
          L"<TextPanel ipos=\"36,447,100,20\" text=\"隐藏正常日志 \" name=\"HideNorTxt\" stretch=\"move_y\" />"

          L"<Button name=\"ExportBtn\" zrol=\"1\" ipos=\"400,439,77,27\" text=\" 导出 \" stretch=\"move_xy\"\
           nor_bd=\"%images_res%erjty_button_normal.png\" \
           pre_bd=\"%images_res%erjty_button_down.png\" \
           ove_bd=\"%images_res%erjty_button_hover.png\" />"

          L"<Button name=\"ClearBtn\" zrol=\"1\" ipos=\"490,439,77,27\" text=\" 清空 \" stretch=\"move_xy\"\
           nor_bd=\"%images_res%erjty_button_normal.png\" \
           pre_bd=\"%images_res%erjty_button_down.png\" \
           ove_bd=\"%images_res%erjty_button_hover.png\" />"
 
          L"<Button name=\"CloseBigBtn\" zrol=\"1\" ipos=\"580,439,77,27\" text=\" 关闭 \" stretch=\"move_xy\"\
           nor_bd=\"%images_res%erjty_button_normal.png\" \
           pre_bd=\"%images_res%erjty_button_down.png\" \
           ove_bd=\"%images_res%erjty_button_hover.png\" />"

          L"<Button name=\"FlushAllBtn\" zrol=\"1\" fadeout=\"10\" ipos=\"310,439,77,27\" text=\" 刷新 \" stretch=\"move_xy\"\
            nor_bd=\"%images_res%erjty_button_normal.png\" \
            pre_bd=\"%images_res%erjty_button_down.png\" \
            ove_bd=\"%images_res%erjty_button_hover.png\" />"

 
       L"</DiyLayout>"
     L"</Dialog>";

//     KReplaceAll(strRet, L"%images_res%", g_strResPath);
//     KReplaceAll(strRet, L">", L">\n");
    OutputDebugStringW(strRet.c_str());
    return strRet;
}