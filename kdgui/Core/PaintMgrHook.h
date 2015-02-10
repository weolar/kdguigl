
#define PaintMgrPaintHook0() \
bool bNeedContinue = true;\
void* pCallBackContext = 0;\
if (m_manager->m_callbacks.m_paint) {\
	m_manager->m_callbacks.m_paint(m_manager->GetWrap(),\
	NULL, m_manager->GetPagePtr(), m_manager->GetHWND(),\
	KDPPaintStepPrePaintToMemoryDC, &pCallBackContext, &bNeedContinue, &(RECT)m_invalidRect, hMemoryDC, psHdc);\
}\
if (!bNeedContinue)\
	goto Exit0;

//////////////////////////////////////////////////////////////////////////

#define PaintMgrPaintHook1() \
if (m_manager->m_callbacks.m_paint) {\
	m_manager->m_callbacks.m_paint(m_manager->GetWrap(),\
	NULL, m_manager->GetPagePtr(), m_manager->GetHWND(),\
	KDPPaintStepPostPaintToMemoryDC, &pCallBackContext, &bNeedContinue, &(RECT)m_invalidRect, hMemoryDC, psHdc);\
}\
if (!bNeedContinue)\
	goto Exit0;

//////////////////////////////////////////////////////////////////////////

#define PaintMgrPaintHook2()\
if (m_manager->m_callbacks.m_paint) {\
	m_manager->m_callbacks.m_paint(m_manager->GetWrap(),\
	NULL, m_manager->GetPagePtr(), m_manager->GetHWND(),\
	KDPPaintStepPostPaintToScreenDC, &pCallBackContext, &bNeedContinue, &(RECT)m_invalidRect, hMemoryDC, psHdc);\
}
