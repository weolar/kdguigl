
#include <UIlib.h>
#ifdef _MSC_VER
#include <tchar.h>
#include <comdef.h>
#endif
#include "StdUtil.h"
#include "skia/include/core/SkStream.h"
#include "wtf/UtilHelp.h"

CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
   m_pVoid(NULL), 
   m_nCount(0), 
   m_iElementSize(iElementSize), 
   m_nAllocated(iPreallocSize) {
   ASSERT(iElementSize>0);
   ASSERT(iPreallocSize>=0);
   if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(WTF::fastMalloc(iPreallocSize * m_iElementSize));
}

CStdValArray::~CStdValArray() {
   if( m_pVoid != NULL ) WTF::fastFree(m_pVoid);
}

void CStdValArray::Empty() {   
   m_nCount = 0;  // NOTE: We keep the memory in place
}

bool CStdValArray::IsEmpty() const {
   return m_nCount == 0;
}

bool CStdValArray::Add(LPCVOID pData) {
   if( ++m_nCount >= m_nAllocated) {
      m_nAllocated *= 2;
      if( m_nAllocated == 0 ) m_nAllocated = 11;
      m_pVoid = static_cast<LPBYTE>(WTF::fastRealloc(m_pVoid, m_nAllocated * m_iElementSize));
      if( m_pVoid == NULL ) return false;
   }
   ::memcpy(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
   return true;
}

bool CStdValArray::Remove(int iIndex) {
	DebugBreak();
	return false;
}

int CStdValArray::GetSize() const {
   return m_nCount;
}

LPVOID CStdValArray::GetBuffer() {
   return static_cast<LPVOID>(m_pVoid);
}

LPVOID CStdValArray::GetAt(int iIndex) const {
   if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
   return m_pVoid + (iIndex * m_iElementSize);
}

LPVOID CStdValArray::operator[] (int iIndex) const {
   ASSERT(iIndex>=0 && iIndex<m_nCount);
   return m_pVoid + (iIndex * m_iElementSize);
}

void CStdValArray::SetSize(int nSize) {
	if (m_nAllocated < nSize)
		ResizePrealloc(nSize);
	
	m_nCount = nSize;
}

bool CStdValArray::ResizePrealloc(int iPreallocSize) {
	if (iPreallocSize <= m_nAllocated)
		return true;
	m_nAllocated = iPreallocSize;

	LPBYTE pVoid = static_cast<LPBYTE>(WTF::fastMalloc(iPreallocSize * m_iElementSize));
	if (!pVoid)
		return false;

	::memcpy(pVoid, m_pVoid, m_iElementSize * m_nCount);
	if (m_pVoid)
		WTF::fastFree(m_pVoid);
	m_pVoid = pVoid;

	return true;
}

UINT WINAPI CStdValArray::WriteData(const unsigned char* pData, UINT uSize) {
	if (m_iElementSize != 1 || uSize > 0xffffff)
		return 0;

	if (m_nAllocated < (int)uSize)
		ResizePrealloc(uSize);

	memcpy(m_pVoid + m_nCount * m_iElementSize, pData, uSize);
	m_nCount += uSize;

	return uSize;
}

unsigned char* WINAPI CStdValArray::ReAlloc(UINT uSize) {
	if (1 != m_iElementSize)
		return 0;

	if (m_nAllocated < (int)uSize)
		ResizePrealloc(uSize);
	m_nCount = uSize;
	return m_pVoid;
}

void WINAPI CStdValArray::Free() {
	if (m_pVoid)
		WTF::fastFree(m_pVoid);
	m_nCount = 0;
}

#if _MSC_VER

CStdValVector::CStdValVector() {
	m_data = NULL;
}

CStdValVector::~CStdValVector() {
	UHDeletePtr(&m_data);
}

UINT CStdValVector::WriteData(const unsigned char* pData, UINT uSize) {
	UHDeletePtr(&m_data);
	if (0 == uSize)
		return 0;

	m_data = new SkMemoryStream(pData, uSize, true);
	return uSize;
}

#endif