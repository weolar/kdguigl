
#include <UIlib.h>
#include "ResSkiaImage.h"
#include "graphics/nanovg/nanovg.h"

#ifdef _MSC_VER

using std::min;
using std::max;
#include "gdiplus.h"
#include "graphics/BMPImageReader.h"
#include "graphics/nanovg/nanovg.h"

static CLSID s_bmpClsid = {0};
static CLSID s_pngClsid = {0};
static bool s_bInitClsid = false;

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j) {
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

static bool InitClsids() {
	if (s_bInitClsid)
		return true;

	if (-1 == GetEncoderClsid(L"image/bmp", &s_bmpClsid))
		return false;

	if (-1 == GetEncoderClsid(L"image/png", &s_pngClsid))
		return false;

	s_bInitClsid = true;

	return true;
}

static void GDIPlusDecoder(CStdValVector* data) {
	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, data->m_data->getLength());
	BYTE* pMem = (BYTE*)::GlobalLock(hMem);
	memcpy(pMem, data->m_data->getMemoryBase(), data->m_data->getLength());

	IStream* pIStream = 0;
	::CreateStreamOnHGlobal(hMem, FALSE, &pIStream);

	Gdiplus::Bitmap* pImgBitmap = Gdiplus::Bitmap::FromStream(pIStream);

	InitClsids();

	pIStream->Release();
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, true, &pIStream);

	pImgBitmap->Save(pIStream, &s_bmpClsid, NULL);

	LARGE_INTEGER liTemp = {0};
	pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
	
	STATSTG stats = {0};
	pIStream->Stat(&stats, 0);
	DWORD dwSize = (DWORD)stats.cbSize.QuadPart;

	delete data->m_data;
	data->m_data = new SkMemoryStream(dwSize);
	pIStream->Read((void *)data->m_data->getMemoryBase(), dwSize, NULL);

	::GlobalUnlock(hMem);
	::GlobalFree(hMem);
	pIStream->Release();

	delete pImgBitmap;
}

#endif // _MSC_VER

int ResSkiaImage::ImageDataToSkBitmap(NVGcontext* ctx, const CStdString& src, CStdValVector* data) {
	int bitmap = 0;
	bool bOpaque = false;

// 	if (!data->m_data)
// 		return 0;
//
// 	SkMemoryStream* stream = data->m_data;
// 	return nvgCreateImageMem(ctx, 0, (unsigned char *)stream->getMemoryBase(), stream->getLength());
	return nvgCreateImageMem(ctx, 0, (unsigned char *)data->GetBuffer(), data->GetSize());

#if 0
	if (-1 != src.Find(L".jpg") || -1 != src.Find(L".png")) { // 这里改为不需要后缀，直接通过文件头判断是何图片类型
		bOpaque = true;
		GDIPlusDecoder(data);
	}

	SkMemoryStream* stream = data->m_data; // GDIPlusDecoder可能会更改m_data
	if (!stream)
		return NULL;

	bitmap = new SkBitmap();
	if (BMPImageReader::decode(false, stream, bitmap))
		return bitmap;
	
	delete bitmap;
	return 0;
#endif

#if 0 // 直接用skia的BMP解压器，会发现png转换而成的BMP32，会丢失alpha通道
	SkImageDecoder* coder = SkImageDecoder::Factory(stream);
	if (!coder) 
		return NULL;

	bitmap = new SkBitmap();

	if (!coder->decode(stream, bitmap, SkBitmap::kARGB_8888_Config, SkImageDecoder::kDecodePixels_Mode)) {
		delete bitmap;
		return NULL;
	}

	bitmap->setIsOpaque(bOpaque);
#endif
	return bitmap;
}
