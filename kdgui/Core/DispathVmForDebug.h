
#ifdef _MSC_VER

BOOL g_bGdiObjectsLeak = FALSE;

static BOOL CheckGDIResLeak() {
	if (1 == GetTickCount()%70) {
		const unsigned int kLotsOfGDIObjects = 5000;
		unsigned int nGdiObjectsNum = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
		if (nGdiObjectsNum == 0 || nGdiObjectsNum > kLotsOfGDIObjects)
			return TRUE;
	}

	return FALSE;
}

#define sq_update_and_juage_isrinning() \
	if (m_scriptMgr && m_scriptMgr->GetScriptDbg()) {\
		sq_rdbg_update(m_scriptMgr->GetScriptDbg());\
	if (!sq_rdbg_isrunning(m_scriptMgr->GetScriptDbg()))\
		return;\
	}\
	if (g_bGdiObjectsLeak)\
		return;\
	g_bGdiObjectsLeak = CheckGDIResLeak();
#else
#define sq_update_and_juage_isrinning() 
#endif
