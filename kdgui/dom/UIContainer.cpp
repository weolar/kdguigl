
#include "StdAfx.h"
#include <tchar.h>
#include "UIContainer.h"
#include "Core/debug.h"

CContainerUI::CContainerUI()
   : m_iPadding(0)
   , m_bAutoDestroy(true)
{
   m_bIsContainer = TRUE;
   m_cxyFixed.cx = m_cxyFixed.cy = 0;
   ::ZeroMemory(&m_rcInset, sizeof(m_rcInset));
}

CContainerUI::~CContainerUI()
{
   RemoveAll();
}

LPCTSTR CContainerUI::GetClass() const
{
   return _T("ContainerUI");
}

LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("Container")) == 0 ) return static_cast<IContainerUI*>(this);
   if( _tcscmp(pstrName, _T("CContainerUI")) == 0 ) return static_cast<CContainerUI*>(this);
   return UINode::GetInterface(pstrName);
}

void CContainerUI::Init()
{
	int i = 0;
	for (i = 0; i < m_items.GetSize(); i++) {
		((UINode*)m_items[i])->Init();
	}
}

UINode* CContainerUI::GetItem(int iIndex) const
{
   if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
   return static_cast<UINode*>(m_items[iIndex]);
}

int CContainerUI::GetCount() const
{
   return m_items.GetSize();
}

bool CContainerUI::AppendChild(UINode* pControl)
{
	// 将子控件添加到队列里来
	if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
	if( m_pManager != NULL ) m_pManager->UpdateLayout();

    return m_items.Add(pControl);
}

bool CContainerUI::RemoveChild(UINode* pControl)
{
   for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
      if( static_cast<UINode*>(m_items[it]) == pControl ) {
         delete pControl;
         return m_items.Remove(it);
      }
   }
   return false;
}

void CContainerUI::RemoveAll()
{
   for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) 
	   delete static_cast<UINode*>(m_items[it]);
   m_items.Empty();

   if( m_pManager != NULL ) m_pManager->UpdateLayout();
}

void CContainerUI::SetAutoDestroy(bool bAuto)
{
   m_bAutoDestroy = bAuto;
}

void CContainerUI::SetInset(SIZE szInset)
{
   m_rcInset.left = m_rcInset.right = szInset.cx;
   m_rcInset.top = m_rcInset.bottom = szInset.cy;
}

void CContainerUI::SetInset(RECT rcInset)
{
   m_rcInset = rcInset;
}

void CContainerUI::SetPadding(int iPadding)
{
   m_iPadding = iPadding;
}

void CContainerUI::SetWidth(int cx)
{
   m_cxyFixed.cx = cx;
}

void CContainerUI::SetHeight(int cy)
{
   m_cxyFixed.cy = cy;
}

void CContainerUI::SetAllVisible(bool bVisible)
{
    //////////////////////////////////////////////////////////////////////////
    // 为了满足tab控件的设置请求，把原本Visible=false的控件设置为false再设置回来的时候应该
    // 是照样维持原样，即false，所以改写本函数。单独再开个TabSetVisible满足原来逻辑
// 	if (m_FalseItems.IsEmpty() && false == bVisible) {
// 		for( int it = 0; it < m_items.GetSize(); it++ ) {
// 			if (static_cast<UINode*>(m_items[it])->IsVisible() != false) {continue;}
// 			m_FalseItems.Add(m_items[it]);
// 		}
// 	}
// 
// 	bool bMustVisibleFalse = false;
// 	if( m_hwndScroll != NULL ) ::ShowScrollBar(m_hwndScroll, SB_CTL, bVisible);
// 	for( int it = 0; it < m_items.GetSize(); it++ ) {
// 		bMustVisibleFalse = false; // 是否真的应该设置为true
// 		if (true == bVisible) {
// 			for( int jt = 0; jt < m_FalseItems.GetSize(); jt++ ) {
// 				if ((m_items[it]) == m_FalseItems[jt]) {bMustVisibleFalse = true;}
// 			}
// 		}
// 		if (false == bVisible || true == bMustVisibleFalse) {
// 			static_cast<UINode*>(m_items[it])->SetVisible(bVisible);
// 		}
// 	}
//	UINode::SetVisible(bVisible);
}

void CContainerUI::SetVisible(bool bVisible)
{
	// Hide possible scrollbar control
	// Hide children as well
	for( int it = 0; it < m_items.GetSize(); it++ ) {
		static_cast<UINode*>(m_items[it])->SetVisible(bVisible);
	}
	UINode::SetVisible(bVisible);
}

int CContainerUI::Event(TEventUI& event)
{
   return UINode::Event(event);
}

SIZE CContainerUI::EstimateSize(SIZE /*szAvailable*/)
{
   return m_cxyFixed;
}

void CContainerUI::SetManager(CPaintManagerUI* pManager, UINode* pParent)
{
   for( int it = 0; it < m_items.GetSize(); it++ ) {
      static_cast<UINode*>(m_items[it])->SetManager(pManager, this);
   }
   UINode::SetManager(pManager, pParent);
}

UINode* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    UINode* pZrolCtl = NULL;
    UINode* pControl = NULL;
    INT Zrol = 0;

    // Check if this guy is valid
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
    if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&BoundingRect(), *(static_cast<LPPOINT>(pData))) ) return NULL;
    if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
        UINode* pControl = UINode::FindControl(Proc, pData, uFlags);
        if( pControl != NULL ) return pControl;
    }
    for( int it = 0; it != m_items.GetSize(); it++ ) {
        if( (uFlags & UIFIND_PASSRESPONSE) != 0 &&
            _tcscmp(((UINode*)(m_items[it]))->GetClass(), _T("MsgResponseUI")) == 0 ) {
            continue;
        }
        pControl = static_cast<UINode*>(m_items[it])->FindControl(Proc, pData, uFlags);

        if( NULL == pControl ) continue;
        CStdString Name = pControl->GetName();
    }
    if( pZrolCtl != NULL ) 
        return pZrolCtl;
    return UINode::FindControl(Proc, pData, uFlags);
}

// void CContainerUI::DoPaint(HDC hDC, const RECT& rcPaint)
// {
//    RECT rcTemp = { 0 };
//    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;
// 
//    CRenderClip clip;
//    CRenderEngineUI::GenerateClip(hDC, m_rcItem, clip);
// 
//    for( int it = 0; it < m_items.GetSize(); it++ ) {
//       UINode* pControl = static_cast<UINode*>(m_items[it]);
// 
//       if( !pControl->IsVisible() ) continue;
// 
//       if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
//       if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
//       pControl->DoPaint(hDC, rcPaint);
//    }
// }