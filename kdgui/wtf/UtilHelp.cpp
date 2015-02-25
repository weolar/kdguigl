#include <UIlib.h>
#include "UtilHelp.h"
#ifndef _MSC_VER
#include "dom/style/NodeStyle.h"
#endif
#include "Graphics/TextRun.h"
#include "graphics/KdPath.h"
#include "Core/UIManager.h"

UINT UHGetNameHash(const SQChar* pstrName) {
	UINT i = 0;
	size_t len = _tcslen(pstrName);
	while( len-- > 0 ) i = (i << 5) + i + pstrName[len];
	return i;
}

UINT UHGetStrHash(const CStdString& pstrName) {
	UINT i = 0;
	size_t len = pstrName.GetLength();
	const SQChar* str = pstrName.GetString();
	while( len-- > 0 ) i = (i << 5) + i + (WCHAR)(str[len]);
	return i;
}

bool UHGetUrlDataFromFile(CStdString& sUrl, CStdValVector& outData) {
#ifdef _MSC_VER
	HANDLE        hFile        = NULL;
	DWORD         bytesReaded  = 0;
	UINT          DataSize     = 8;
	bool          bRet         = false;
	LARGE_INTEGER FileSize     = {0};

	UHDeletePtr(&outData.m_data);

	sUrl.MakeLower();
	sUrl.Replace(_SC("/"), _SC("\\"));
	if (-1 != sUrl.Find(_SC("file:\\\\\\"), 0))
		sUrl.Delete(0, 8);

	hFile = CreateFileW(sUrl.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(!hFile || INVALID_HANDLE_VALUE == hFile)
		return false; 

	bRet = !!GetFileSizeEx(hFile, &FileSize);
	if (!bRet || (0 == FileSize.HighPart && 0 == FileSize.LowPart) || 0 != FileSize.HighPart) {
		bRet = false;
		goto Exit0;
	}
	
	outData.m_data = new SkMemoryStream(FileSize.LowPart + 10);
	memset((LPVOID)outData.m_data->getMemoryBase(), 0, FileSize.LowPart + 10);

	if(!::ReadFile(hFile, (LPVOID)(outData.m_data->getMemoryBase()), FileSize.LowPart, &bytesReaded, NULL) ||
		bytesReaded > FileSize.LowPart)
		goto Exit0;
	//outData.SetSize(bytesReaded);
	
	bRet = true;

Exit0:
	if (hFile && INVALID_HANDLE_VALUE != hFile) { 
		::CloseHandle(hFile);
		hFile = NULL;
	}

	return bRet;
#else
	DebugBreak();
	return false;
#endif
}

BOOL UHStringSplit(CStdString& strData, CStdString& strSplit, WTF::Vector<CStdString>& vecList) {
	int			nIndex		= -1;
	int			nStartIndex	= 0;
	int			nCount		= 0;
	CStdString  strItem;
	CStdString  strBuf = strData;

	vecList.clear();
	strBuf = strData;

	do {
		nIndex = strBuf.Find(strSplit, nStartIndex);
		if (nIndex == -1) {
			if (nStartIndex <= strBuf.GetLength()) {
				nCount = strBuf.GetLength() - nStartIndex;
				strItem = strBuf.Mid(nStartIndex, nCount);

				//strItem.Trim();
				vecList.push_back(strItem);
			}

			break;
		}

		nCount = nIndex - nStartIndex;
		//strItem.Trim();
		strItem = strBuf.Mid(nStartIndex, nCount);
		vecList.push_back(strItem);

		nStartIndex = nIndex + 1;
		if (nStartIndex > strBuf.GetLength())
			break;
	} while(nIndex != -1);

	return TRUE;
}

SkColor UHParseColorToRGB(const CStdString& color) {
	CStdString sColor(color);
	sColor.Replace(_SC(" "), _SC(""));

	int r = 0, g = 0, b = 0, a = 0;
	if (sColor == _SC("white"))
		return 0xffffffff;
	else if (sColor == _SC("red"))
		return 0xff0000ff;

	SkColor rgb = 0xFF000000;
	if (-1 != sColor.Find(_SC("#"))) {
		int nRet = _tsscanf(sColor.GetString(), _SC("#%x"), &rgb);
		if (1 != nRet) 
			return rgb;
	} else if (-1 != sColor.Find(_SC("argb("))) {
		int nRet = _tsscanf(sColor.GetString(), _SC("argb(%d,%d,%d,%d)"), &a, &r, &g, &b);
		if (4 != nRet) 
			return rgb;
		rgb = RGB(b, g, r);
	} else if (-1 != sColor.Find(_SC("rgb("))) {
		
		int nRet = _tsscanf(sColor.GetString(), _SC("rgb(%d,%d,%d)"), &r, &g, &b);
		if (3 != nRet) 
			return rgb;
		rgb = RGB(b, g, r); // skia用的顺序和windows的刚好相反，注意一下
	} else { // 除此之外，全部当成数字处理
		_tsscanf(sColor.GetString(), _SC("%x"), &rgb);
		return rgb;
	}
	return rgb;
}

bool UHLoadRes(LPCTSTR pSrc, CPageManager* pManager, CStdValVector& data) {
	CStdString src(pSrc);
	
	CStdString newSrc = src;
	bool bNoProtocol = -1 == src.Find(_SC("/")) && -1 == src.Find(_SC("\\"));
	if (bNoProtocol) {// 如果没写明是什么协议
		/*newSrc = */newSrc.Format(_SC("没写明协议：%s \n"), newSrc.GetString());
		OutputDebugString(newSrc);
		/*newSrc = */newSrc.Format(_SC("res:///%s"), src.GetString());
	}
	
	HRESULT hr = -1;
	if (pManager->m_callbacks.m_resHandle)
		hr = pManager->m_callbacks.m_resHandle(pManager->GetWrap(), NULL, pManager->GetPagePtr(), pManager->GetPaintWindow(), newSrc.GetString(), (IKdGuiBuffer*)&data);
	if (0 == hr)
		return true;

#ifdef _MSC_VER
	if (bNoProtocol) {
		if (GetModuleFileName(NULL, newSrc.GetBuffer(MAX_PATH + 1), MAX_PATH)) {
			newSrc.ReleaseBuffer();
			if (_SC('\\') != newSrc[newSrc.GetLength() - 1])
				newSrc += _SC('\\');
			newSrc = newSrc + src;
		}
	}
#endif

	// 还没找到，看是否可以在本地文件中找
	if (UHGetUrlDataFromFile(newSrc, data))
		return true;

// 	KDASSERT(FALSE);
	/*src = */src.Format(_SC("缺少资源: %s, doc:%s \n"), src.GetString(), pManager->m_docURI.GetString());
	OutputDebugString(src.GetString());

	if (pManager->m_callbacks.m_error)
		pManager->m_callbacks.m_error(pManager->GetVM(), _SC("%ws"), src.GetString());
	return false;
}

// 哪些事情是有捕获和冒泡两阶段
bool UHCanEventUseCapture (UIEventType evt) {
	switch(evt) {
	case UIEVENT_MOUSEMOVE:
	case UIEVENT_MOUSEHOVER:

	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSELEAVE:

	case UIEVENT_MOUSEDOWN:
	case UIEVENT_MOUSEUP:
	case UIEVENT_CLICK:

	case UIEVENT_KILLFOCUS:
	case UIEVENT_SETFOCUS:
	case UIEVENT_MOUSEWHEEL:

	case UIEVENT_TOUCHBEGIN:
	case UIEVENT_TOUCHMOVE:
	case UIEVENT_TOUCHEND:
	case UIEVENT_TOUCANCEL:
		return true;
	}

	return false;
}

static WTF::HashMap<UINT, std::pair<CStdString, UIEventType> >* eventNameMap = 0;

static void InsertEventNameMap (const CStdString& eventName, UIEventType eventType) {
	eventNameMap->insert(UHGetNameHash(eventName), std::pair<CStdString, UIEventType>(eventName, eventType));
}

UIEventType UHEventNameToEnum (const SQChar* eventName) {
	if (!eventNameMap) {
		eventNameMap = new WTF::HashMap<UINT, std::pair<CStdString, UIEventType> >();
		InsertEventNameMap(_SC("mousemove"), UIEVENT_MOUSEMOVE);
		InsertEventNameMap(_SC("mouseout"), UIEVENT_MOUSELEAVE);
		InsertEventNameMap(_SC("mouseover"), UIEVENT_MOUSEENTER);
		InsertEventNameMap(_SC("mousedown"), UIEVENT_MOUSEDOWN);
		InsertEventNameMap(_SC("mouseup"), UIEVENT_MOUSEUP);
		InsertEventNameMap(_SC("click"), UIEVENT_CLICK);
		InsertEventNameMap(_SC("focus"), UIEVENT_SETFOCUS);
		InsertEventNameMap(_SC("blur"), UIEVENT_KILLFOCUS);
		InsertEventNameMap(_SC("mousewheel"), UIEVENT_MOUSEWHEEL);

		InsertEventNameMap(_SC("touchbegin"), UIEVENT_TOUCHBEGIN);
		InsertEventNameMap(_SC("touchmove"), UIEVENT_TOUCHMOVE);
		InsertEventNameMap(_SC("touchend"), UIEVENT_TOUCHEND);
		InsertEventNameMap(_SC("touchcancel"), UIEVENT_TOUCANCEL);
	}

	if (!eventName)
		return UIEVENT__LAST;

	WTF::HashMap<UINT, std::pair<CStdString, UIEventType> >::iterator it = eventNameMap->find(UHGetNameHash(eventName));
	if (it != eventNameMap->end())
		return it->second.second;
	
	return UIEVENT__LAST;
}

const SQChar* UHEnumToEventName (UIEventType evt) {
	WTF::HashMap<UINT, std::pair<CStdString, UIEventType> >::iterator it = eventNameMap->begin();
	for (; it != eventNameMap->end(); ++it)
		if (it->second.second == evt)
			return it->second.first.GetString();
	return NULL;
}

bool UHIsMouseEvent (UIEventType evt) {
	switch(evt) {
	case UIEVENT_MOUSEMOVE:
	case UIEVENT_MOUSEHOVER:

	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSELEAVE:

	case UIEVENT_MOUSEDOWN:
	case UIEVENT_MOUSEUP:
	case UIEVENT_CLICK:

	case UIEVENT_TOUCHBEGIN:
	case UIEVENT_TOUCHMOVE:
	case UIEVENT_TOUCHEND:
	case UIEVENT_TOUCANCEL:
		return true;
	}

	return false;
}

bool UHIsInputEvent (UIEventType evt) {
	switch(evt) {
	case UIEVENT_MOUSEMOVE:
	case UIEVENT_MOUSEHOVER:

	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSELEAVE:

	case UIEVENT_MOUSEDOWN:
	case UIEVENT_MOUSEUP:
	case UIEVENT_CLICK:
		return true;
	}

	return false;
}

bool UHCalculateRectPath(const CStdString& clipString, KdPath& path) {
	if (clipString.IsEmpty() || clipString.GetLength() <= sizeof(_SC("rect(")))
		return false;

	WTF::Vector<CStdString> result; // rect(20,20,30,30)
	CStdString s = clipString;
	s.Delete(0, sizeof(_SC("rect(")) - 1);
	s.Trim();

	CStdString comma(_SC(","));
	UHStringSplit(s, comma, result); // x, y, w, h 格式
	if (result.size() < 4)
		return false;

	float x = (float)_tcstod(result[0], (SQChar**)NULL);
	float y = (float)_tcstod(result[1], (SQChar**)NULL);
	float w = (float)_tcstod(result[2], (SQChar**)NULL);
	float h = (float)_tcstod(result[3], (SQChar**)NULL);
	float rx = 0.0;
	float ry = 0.0;
	if (5 == result.size())
		rx = (float)_tcstod(result[4], (SQChar**)NULL);
	if (6 == result.size())
		ry = (float)_tcstod(result[5], (SQChar**)NULL);

	FloatRect clipBoundaries(x, y, w, h);
	path.addRect(clipBoundaries);

	return true;
}

static WCHAR* Int2Str(int nIn, WCHAR* strOut, int nOutLen) {
	int nCount = 0;
	int nTemp;
	int nFlag = 0;
	WCHAR cTemp;
	int i = 0;
	if (!strOut)
		return NULL;

	// 如果为0
	if (nIn == 0) {
		strOut[0] = _SC('0');
		strOut[1] = _SC('\0');
		return strOut;
	}

	// 如果为负数
	if (nIn < 0) {
		nIn = -nIn;
		strOut[nCount] = _SC('-');
		nCount++;
		nFlag = 1;
	}

	nTemp = nIn;

	// 转换
	while(nTemp > 0) {
		if (nCount > nOutLen - 1)
			return NULL;

		strOut[nCount] = (WCHAR)(nTemp%10 + _SC('0'));
		nCount++;
		nTemp = nTemp/10;
	}

	// 结束符
	strOut[nCount] = _SC('\0');

	// 如果是负数从strOut[1]开始
	if (nFlag == 0) {
		for (i = 0; i < nCount/2; i++) {
			cTemp = strOut[i];
			strOut[i] = strOut[nCount-1-i];
			strOut[nCount-1-i]=cTemp;
		}
	} else {
		for (i = 1; i < nCount/2; i++) {
			cTemp = strOut[i];
			strOut[i] = strOut[nCount-i];
			strOut[nCount-i]=cTemp;
		}
	}

	return strOut;
}  

float UHFastStrToFloat(const SQChar* pstrfloat) {
	if (!pstrfloat)
		return 0.0;

	bool bNegative = false;
	bool bDec = false;

	const SQChar* pSor = 0;
	SQChar chByte = _SC('0');
	float fInteger = 0.0;
	float fDecimal = 0.0;
	float fDecPower = 0.1f;

	// 进行首位判断，判断是否是负数
	if (pstrfloat[0] == _SC('-')) {
		bNegative = true;
		pSor = pstrfloat + 1;
	} else {
		bNegative = false;
		pSor = pstrfloat;
	}

	while (*pSor != _SC('\0')) {
		chByte = *pSor;

		if (bDec) { // 小数  
			if (chByte >= _SC('0') && chByte <= _SC('9')) {  
				fDecimal += (chByte - _SC('0')) * fDecPower;  
				fDecPower = fDecPower * 0.1f;  
			} else 
				return (bNegative ? -(fInteger +  fDecimal) : fInteger + fDecimal);  
		} else { // 整数  
			if (chByte >= _SC('0') && chByte <= _SC('9')) {  
				fInteger = fInteger * 10.0f + chByte - _SC('0');  
			} else if (chByte == _SC('.')) {  
				bDec = true;  
			} else
				return (bNegative ? -fInteger : fInteger);
		}

		pSor++;
	}

	return (bNegative ? -(fInteger +  fDecimal) : fInteger + fDecimal);
}

SQChar* UHFastFloat2StrFormat(float f, SQChar* buf) {
	int arg = (int)(f*100);
	memset(buf, 0, nUHFastFloat2StrFormatBufLen*sizeof(SQChar));
	sprintf(buf, /*nUHFastFloat2StrFormatBufLen,*/ _SC("%f"), arg/100.0f);
	//DebugBreak();

#if 0
	int arg = (int)(f*10);
	if (!Int2Str(arg, buf, nUHFastFloat2StrFormatBufLen - 1))
		*(int*)0 = 0;

	//wcscat_s(buf, nUHFastFloat2StrFormatBufLen, L" ");

	int nLen = (int)wcslen(buf);
	if (2 == nLen)
		return buf;

	if (nLen > nUHFastFloat2StrFormatBufLen - 10)
		*(int*)0 = 0;

	for (int i = nLen; i > nLen - 2; --i) {
		buf[i] = buf[i - 1];
	}
	buf[nLen - 2] = L'.';
	buf[nLen] = 0;
#endif
	return buf;
}

// 把多个空格缩为一个
CStdString UHTrimBlankSpace(const CStdString& str) {
	CStdString newStr;
	for (int i = 0; i < str.GetLength(); ++i) {
		if (_SC(' ') == str[i]) {
			newStr += _SC(' ');
			while (_SC(' ') == str[++i]) {}
			--i;
		} else
			newStr += str[i];
	}
	return newStr;
}