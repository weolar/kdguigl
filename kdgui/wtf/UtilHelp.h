#ifndef UtilHelp_h
#define UtilHelp_h

#include "core/UIEventType.h"

template <typename T>
void UHDeletePtr(T** ptr) {
	if (*ptr)
		delete *ptr;
	*ptr = NULL;
}

class CPageManager;

#ifdef _MSC_VER
class CStdValVector;
#endif

bool UHGetUrlDataFromFile(CStdString& sUrl, CStdValVector& OutData);

UINT UHGetNameHash(const SQChar* pstrName);
UINT UHGetStrHash(const CStdString& pstrName);

BOOL UHStringSplit(CStdString& strData, CStdString& strSplit, WTF::Vector<CStdString>& vecList);
CStdString UHTrimBlankSpace(const CStdString& str);

SkColor UHParseColorToRGB(const CStdString& sColor);

bool UHLoadRes(LPCTSTR pSrc, CPageManager* pManager, CStdValVector& data);

bool UHCanEventUseCapture (UIEventType evt);

UIEventType UHEventNameToEnum (const SQChar* eventName);
const SQChar* UHEnumToEventName (UIEventType evt);
bool UHIsMouseEvent (UIEventType evt);

class KdPath;
bool UHCalculateRectPath(const CStdString& clipString, KdPath& path);

const int nUHFastFloat2StrFormatBufLen = 60;
float UHFastStrToFloat(const SQChar* pstrfloat);
SQChar* UHFastFloat2StrFormat(float f, SQChar* buf);

#endif // UtilHelp_h
