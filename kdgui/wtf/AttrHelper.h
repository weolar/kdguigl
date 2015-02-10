#ifndef attrhelper_h_
#define attrhelper_h_
#include "kuidefine.h"

#pragma warning( disable : 4102 )

inline int _AttrDDXItem(CStdString &strAttr, CStdString &strValue, LPCTSTR strName, CStdString &strVar, BOOL bSet)
{
	if (strAttr == strName)
	{
		if (bSet)
			strVar = strValue;
		else
			strValue = strVar;
		
		return 0;
	}

	return -1;
}

inline int _AttrDDXItem(CStdString &strAttr, CStdString &strValue, LPCTSTR strName, int &nVar, BOOL bSet)
{
	if (strAttr == strName)
	{
		if (bSet)
			nVar = _tcstol(strValue, NULL, 10);
		else
			strValue.Format(L"%d", nVar);

		return 0;
	}

	return -1;
}

template<typename T>
inline int _AttrDDXItem(CStdString &strAttr, CStdString &strValue, LPCTSTR strName, T &TObject, BOOL bSet)
{
	if (strAttr == strName)
	{
		if (bSet)	
			TObject.ParseString(strValue);
		else
			strValue = TObject.ToString();

		return 0;
	}

	return -1;
}

#define KDWIN_ATTR_DDX_BEGIN()\
	virtual int _AttrDDX(CStdString &strAttr, CStdString &strValue, BOOL bSet){\
		int nReturn = -1;

#define KDWIN_ATTR_DDX_END()\
	{}\
Exit0:\
	return nReturn;}

#define KDWIN_ATTR_DDX(strVarName, strVar) \
	nReturn = _AttrDDXItem(strAttr, strValue, strVarName, strVar, bSet);\
	if (nReturn == 0) goto Exit0;

#define KDWIN_ATTR_DDX_CHAIN(parent)        \
	nReturn = parent::_AttrDDX(strAttr, strValue, bSet);\
	if (nReturn == 0) goto Exit0;\
	else

#endif // attrhelper_h_