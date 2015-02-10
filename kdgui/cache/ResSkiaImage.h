#ifndef ResSkiaImage_h
#define ResSkiaImage_h

struct NVGcontext;
class ResSkiaImage {
public:
	static int ImageDataToSkBitmap(NVGcontext* ctx, const CStdString& src, CStdValVector* data);
};

#endif // ResSkiaImage_h