#ifndef CSTRING_H
#define CSTRING_H

#ifndef _MSC_VER

#include <algorithm>
#include <string>
#include <cctype>

#include <stdarg.h>
#include <assert.h>

#ifndef ASSERT

#ifndef		_DEBUG
#define	ASSERT(expr)
#else
#define	ASSERT(expr)		assert(expr)
#endif

#endif

/**
 * 简易CString�?
 * @author singun
 */
class CString : public std::string
{
public:
	void Append( const char * pstr )
	{
		this->append(pstr);
		this->TrimRight('\0');
	}
	void Append( const CString& string1 )
	{
		this->append(string1, string1.GetLength());
	}
	void Append( const CString& string1, int nLen )
	{
		this->append(string1.Left(nLen));
	}
	void Append( const char * pstr, int nLen )
	{
		this->append(pstr, nLen);
		this->TrimRight('\0');
	}
	void AppendChar( const char sz )
	{
		this->append(&sz);
	}
	int Delete( int nIndex, int nCount = 1 )
	{
		this->erase(nIndex,nCount);
		return this->GetLength();
	}
	int Insert( int nIndex, const char * pstr )
	{
		this->insert(nIndex,pstr);
		return this->GetLength();
	}
	int Insert( int nIndex, char ch )
	{
		CString strTmp(ch);
		this->insert(nIndex,strTmp);
		strTmp.Empty();
		return this->GetLength();
	}
	int Remove( char ch )
	{
		CString::iterator iter;
		int count = 0;
		for(iter = this->begin(); iter != this->end();iter ++)
		{
			if(*iter == ch)
			{
				this->erase(iter);count++;
			}
		}
		return count;
	}
	void MakeReverse( )
	{
		CString strTmp;
		CString::iterator iter;
		iter=this->end();
		iter--;
		for(; iter != this->begin(); iter--)
		{
			strTmp += *iter;
		}
		strTmp += *iter;
		*this = strTmp;
		strTmp.Empty();
	}
	int Find( char ch ) const
	{
		return this->find(ch);
	}
	int Find( const char * lpszSub ) const
	{
		return this->find(lpszSub);
	}
	int Find( char ch, int nStart ) const
	{
		return this->find(ch,nStart);
	}
	int Find( const char * pstr, int nStart ) const
	{
		return this->find(pstr,nStart);
	}
	int ReverseFind( char ch ) const
	{
		return this->find_last_of(ch);
	}
	int FindOneOf( const char * lpszCharSet ) const
	{
		return this->find_first_of(lpszCharSet);
	}
	int Format(const char* pstrFormat, ... )
	{///本函数仅仅支持ANSI标准字符�?'%[flags] [width] [.precision] [{h | l | I64 | L}]type'
		ASSERT(pstrFormat!=NULL);
		this->Empty();

		va_list argList;
		va_start(argList,pstrFormat);
		int nMaxLen = 0;
		for (const char * p = pstrFormat; *p != '\0';p++ )
		{
			if (*p != '%' || *(++p) == '%')
			{// 如果不是'%'就直接累计长度，如果�?%%'也使长度�?
				nMaxLen += 1;
				continue;
			}
			int nItemLen = 0; //用来保存每个参数的长�?
			int nWidth = 0; //用来保存每一项的宽度
			for (; *p != '\0'; p ++)
			{
				if (*p == '#')
					nMaxLen += 2;   // 处理 '0x'
				else if (*p == '*')
					nWidth = va_arg(argList, int);  //如：'%5f' 中的5
				else if (*p == '-' || *p == '+' || *p == '0'|| *p == ' ')
					continue;
				else // 不是标志字符就退出循�?
					break;
			}
			if (nWidth == 0)
			{ //提取宽度
				nWidth = atoi(p);
				for (; *p != '\0' && isdigit(*p); p ++)
					;
			}
			ASSERT(nWidth >= 0);//有效宽度
			int nPrecision = 0; //精度位数
			if (*p == '.')
			{
				p++;// 跳过 '.'字符 (宽度.精度)
				if (*p == '*')
				{ //有参数给�?
					nPrecision = va_arg(argList, int);
					p ++;// 取得精度，跳过字�?
				}
				else
				{ //在格式串中有宽度
					nPrecision = atoi(p);
					for (; *p != '\0' && isdigit(*p);	p ++)
						;
				}
				ASSERT(nPrecision >= 0);//有效宽度
			}
			switch (*p)
			{
				case 'h':     //short int �?
					p ++;
					break;
				case 'l':	 //long double �?
					p ++;
					break;
				case 'F':	 //近指�?
				case 'N':	//远指�?
				case 'L':	//long double �?
					p++;
					break;
			}
			switch (*p)
			{
				case 'c':   //// 单个字符
				case 'C':
					nItemLen = 2;
					va_arg(argList, int);
					break;
				case 's':	//// 字符�?
				case 'S':
					nItemLen = strlen(va_arg(argList, const char*));
					nItemLen = ((1) > (nItemLen)) ? (1) : (nItemLen);//如果是空串就使用1 即保�?\0'
					break;
			}
			if (nItemLen != 0)
			{
				nItemLen = ((nItemLen) > (nWidth)) ? (nItemLen) : (nWidth);//使用大�?
				if (nPrecision != 0)
					nItemLen = ((nItemLen) < (nPrecision)) ? (nItemLen) : (nPrecision);
			}
			else
			{
				switch (*p)
				{
					case 'd':    //整数的处�?
					case 'i':
					case 'u':
					case 'x':
					case 'X':
					case 'o':
						va_arg(argList, int);
						nItemLen = 32;  //四字�?
						nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//使用大�?
						break;
					case 'e':	//浮点�?
					case 'f':
					case 'g':
					case 'G':
						va_arg(argList, double);
						nItemLen = 32;//四字�?
						nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//使用大�?
						break;
					case 'p':	//指针
						va_arg(argList, void*);
						nItemLen = 32;
						nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//使用大�?
						break;
					case 'n':
						va_arg(argList, int*); //指向整数的指�?见BorlanderC++3.1库函数P352
						break;
					default:
						ASSERT(false);  //不能处理的格式，给出警告
						break;
					}
				}
				nMaxLen += nItemLen;//把该项的长度累计
			}
			va_end(argList);
			va_start(argList, pstrFormat);  // 重新开始提取参�?
			char* ch = new char[nMaxLen+1]; //分配内存
			vsprintf(ch, pstrFormat, argList);
			this->append(ch); //加到string的尾�?
			delete[] ch; //释放内存
			va_end(argList);
			return nMaxLen;
	}
	int GetLength() const
	{
		return this->length();
	}
	CString Left(int nCount) const
	{
		if (nCount <=0)
			return CString("");
		CString strTmp;
		strTmp = this->substr(0,nCount);
		return strTmp;
	}
	CString Right(int nCount) const
	{
		if (nCount <=0)
			return CString("");
		CString strTmp;
		if (nCount > GetLength())
			strTmp = this->substr(0);
		else
			strTmp = this->substr(GetLength()-nCount);
		return strTmp;
	}
	CString Mid(int nFirst) const
	{
		CString strTmp;
		if (nFirst >= GetLength())
			return CString("");
		if (nFirst <= 0)
			strTmp = this->substr(0);
		else
			strTmp = this->substr(nFirst);
		return strTmp;
	}
	CString Mid( int nFirst, int nCount) const
	{
		if (nCount <= 0)
			return CString("");
		if (nFirst >= GetLength())
			return CString("");
		CString strTmp;
		if (nFirst <= 0)
			strTmp = this->substr(0,nCount);
		else
			strTmp = this->substr(nFirst,nCount);
		return strTmp;
	}
	CString& operator=(const std::string str)
	{
		if (this->compare(str) == 0) return *this;
		this->assign(str);
		return *this;
	}
	CString& operator=(char ch)
	{
		this->Empty();
		this->insert(this->begin(),ch);
		return *this;
	}
	CString& operator =( const char * lpsz )
	{
		this->Empty();
		this->append(lpsz);
		this->TrimRight('\0');
		return *this;
	}
	bool operator==(const CString& string1) const
	{
		if (this->compare(string1) == 0)
			return true;
		return false;
	}
	bool operator==(const char * lpsz) const
	{
		if (this->compare(lpsz) == 0)
			return true;
		return false;
	}
	void MakeUpper()
	{
		std::transform(this->begin (),
			this->end (),this->begin (),
			::toupper);
	}
	void MakeLower()
	{
		std::transform(this->begin (),
			this->end (),this->begin (),
			::tolower);
	}
	bool IsEmpty( ) const
	{
		return this->empty();
	}
	void Empty( )
	{//清除
		this->erase(this->begin(),this->end());
		if (strBuffer != NULL)
			delete []strBuffer;
	}
	char GetAt( int nIndex ) const
	{
		return this->at(nIndex);
	}
	char operator []( int nIndex ) const
	{
		return this->at(nIndex);
	}
	void SetAt( int nIndex, char ch )
	{
		this->at(nIndex) = ch;
	}
	operator const char * ( ) const
	{
		return this->c_str();
	}
	friend CString operator + (const CString& string1, const CString& string2)
	{
		CString str;
		str.append(string1);
		str.append(string2);
		return str;
	}
	friend CString operator + ( const CString& string1, char ch )
	{
		CString str;
		str.append(string1);
		str.insert(str.end(),ch);
		return str;
	}
	friend CString operator + ( const CString& string1, char* ch )
	{
		CString str;
		str.append(string1);
		str.append(ch);
		return str;
	}
	int Compare( const char * lpsz ) const
	{
		CString str;
		str.append(lpsz);
		return this->compare(str);
	}
	int Compare( const CString& string1 ) const
	{
		return this->compare(string1);
	}
	int CompareNoCase( const char * lpsz ) const
	{
		CString str,strThis;
		str.append(lpsz);
		strThis = (*this);
		str.MakeLower();
		strThis.MakeLower();
		return strThis.compare(str);
	}
	int CompareNoCase( const CString& string1 ) const
	{
		CString str,strThis;
		str = string1;
		strThis = (*this);
		str.MakeLower();
		strThis.MakeLower();
		return strThis.compare(str);
	}
	void Trim()
	{
		TrimLeft();
		TrimRight();
	}
	void TrimRight( )
	{
		TrimRight (' ');
	}
	void TrimLeft( )
	{
		TrimLeft(' ');
	}
	void Trim( char chTarget )
	{
		TrimLeft( chTarget );
		TrimRight( chTarget );
	}
	void TrimLeft( char chTarget )
	{
		std::string::size_type pos;
		pos = this->find_first_not_of(chTarget);
		if (pos == 0) return;
		this->erase(this->begin(),this->begin()+pos);
	}
	void TrimRight( char chTarget )
	{
		std::string::size_type pos;
		pos = this->find_last_not_of(chTarget);
		++pos;
		if (pos == this->GetLength())
			return;
		this->erase(this->begin()+pos,this->end());
	}
	void Replace( char chOld, char chNew )
	{
		for(int i=0;i<this->GetLength();i++)
		{
			if (this->at(i) == chOld)
				this->at(i) = chNew;
		}
	}
	void Replace(const char* chOld,const char* chNew )
	{
		int index = this->find(chOld);
		while (index > -1)
		{
			this->erase(index,strlen(chOld));
			this->insert(index,chNew);
			index = this->find(chOld);
		}
	}
 	char * GetBuffer( int nMinBufLength );
// 	{
// 		const char* strTmp = this->c_str();
// 		int nCount = strlen(strTmp);
// 		if (nMinBufLength == 0)
// 			nMinBufLength = nCount;
// 
// 		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Into GetBuffer");
// 
// 		if (nCount > nMinBufLength)
// 			return NULL;
// 		if (strBuffer != NULL) {__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "strBuffer:%x", strBuffer);
// 			delete []strBuffer;}
// 
// 		strBuffer = new char[nMinBufLength + 1];
// 		strcpy(strBuffer, strTmp);
// 		strBuffer[nMinBufLength] = '\0';
// 		return strBuffer;
// 	}
	char* GetBuffer()
	{
		return GetBuffer(0);
	}
	const char*  GetString() const
	{
		return this->c_str();
	}

	void ReleaseBuffer( int nNewLength = -1 );
// 	{
// 		if (strBuffer != NULL)
// 		{
// 			this->Empty();
// 			this->Append(strBuffer);
// 			delete []strBuffer;
// 		}
// 		this->TrimRight('\0');
// 	}
	CString(const CString& string1)
	{
		strBuffer = NULL;
		this->append(string1);
	}
	CString(const char *ch)
	{
		strBuffer = NULL;
		if(ch != NULL)
		{
			this->Append(ch);
		}
	}
	CString(const char ch)
	{
		strBuffer = NULL;
		*this += ch;
	}
	CString()
	{
		strBuffer = NULL;
	}
	CString(const char *ch, int len)
	{
		strBuffer = NULL;
		this->Append(ch, len);
	}
	~CString()
	{
		if (strBuffer != NULL)
			delete []strBuffer;
	}

private:
	char *strBuffer;
};

// typedef CString	 	CStringW;
// typedef CString	 	CStringA;

#endif // _MSC_VER

#endif

