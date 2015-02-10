#if !defined(AFX_INTERNAL_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_INTERNAL_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if 0



#include <e:\Program Files\Microsoft DirectX SDK (March 2009)\Include\d3d9.h>
#include <math.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//

template< class T >
class CSafeRelease
{
public:
   CSafeRelease(T* p) : m_p(p) { };
   ~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
   T* Detach() { T* t = m_p; m_p = NULL; return t; };
   T* m_p;
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CAnimationSpooler
{
public:
   CAnimationSpooler();
   ~CAnimationSpooler();

   enum { MAX_BUFFERS = 40 };

   bool Init(HWND hWnd);
   bool PrepareAnimation(HWND hWnd);
   void CancelJobs();
   bool Render();

   bool IsAnimating() const;
   bool IsJobScheduled() const;
   bool AddJob(CAnimJobUI* pJob);

protected:
   void Term();

   COLORREF TranslateColor(LPDIRECT3DSURFACE9 pSurface, COLORREF clr) const;
   bool SetColorKey(LPDIRECT3DTEXTURE9 pTexture, LPDIRECT3DSURFACE9 pSurface, int iTexSize, COLORREF clrColorKey);

   bool PrepareJob_Flat(CAnimJobUI* pJob);
   bool RenderJob_Flat(const CAnimJobUI* pJob, LPDIRECT3DSURFACE9 pSurface, DWORD dwTick);

protected:
   struct CUSTOMVERTEX 
   {
      FLOAT x, y, z;
      FLOAT rhw;
      DWORD color;
      FLOAT tu, tv;
   };
   typedef CUSTOMVERTEX CUSTOMFAN[4];

   HWND m_hWnd;
   bool m_bIsAnimating;
   bool m_bIsInitialized;
   CStdPtrArray m_aJobs;
   D3DFORMAT m_ColorFormat;
   LPDIRECT3D9 m_pD3D;
   LPDIRECT3DDEVICE9 m_p3DDevice;
   LPDIRECT3DSURFACE9 m_p3DBackSurface;
   //
   LPDIRECT3DVERTEXBUFFER9 m_p3DVertices[MAX_BUFFERS];
   LPDIRECT3DTEXTURE9 m_p3DTextures[MAX_BUFFERS];
   CUSTOMFAN m_fans[MAX_BUFFERS];
   int m_nBuffers;
};


#endif // !defined(AFX_INTERNAL_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

#endif // if 0