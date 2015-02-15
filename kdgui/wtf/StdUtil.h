#if !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
#define AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_

#pragma once

#include <ASSERT.h>
#include <wtf/Assertions.h>

class CPageManager;

class CStdPtrArray
{
public:
   CStdPtrArray(int iPreallocSize = 0);
   virtual ~CStdPtrArray();

   void Empty();
   void Resize(int iSize);
   bool IsEmpty() const;
   int Find(LPVOID iIndex) const;
   bool Add(LPVOID pData);
   bool SetAt(int iIndex, LPVOID pData);
   bool InsertAt(int iIndex, LPVOID pData);
   bool Remove(int iIndex);
   int GetSize() const;
   LPVOID* GetBuffer();

   LPVOID GetAt(int iIndex) const;
   LPVOID operator[] (int nIndex) const;

protected:
   LPVOID* m_ppVoid;
   int m_nCount;
   int m_nAllocated;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class IKdGuiBuffer {
public:
	//virtual UINT WINAPI WriteData(const unsigned char* pData, UINT uSize) = 0;
	virtual unsigned char* WINAPI ReAlloc(UINT uSize) = 0;
	virtual void WINAPI Free() = 0;
};

class CStdValArray : public IKdGuiBuffer
{
public:
   CStdValArray(int iElementSize = 1, int iPreallocSize = 0);
   virtual ~CStdValArray();

   void Empty();
   bool IsEmpty() const;
   bool Add(LPCVOID pData);
   bool Remove(int iIndex);
   int GetSize() const;
   LPVOID GetBuffer();
   void SetSize(int nSize);
   bool ResizePrealloc(int iPreallocSize);

   LPVOID GetAt(int iIndex) const;
   LPVOID operator[] (int nIndex) const;

   virtual unsigned char* WINAPI ReAlloc(UINT uSize);
   virtual UINT WINAPI WriteData(const unsigned char* pData, UINT uSize);
   virtual void WINAPI Free();

protected:
   LPBYTE m_pVoid;
   int m_iElementSize;
   int m_nCount;
   int m_nAllocated;
};

#if _MSC_VER

class SkMemoryStream;

class CStdValVector : public IKdGuiBuffer {
public:
	CStdValVector();
	~CStdValVector();
	virtual UINT WINAPI WriteData(const unsigned char* pData, UINT uSize);
	SkMemoryStream* m_data;
};
#else

typedef CStdValArray CStdValVector;

#endif

//typedef CString CStdString;

#endif // !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
