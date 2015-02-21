
struct TextRun;

#ifdef _MSC_VER

IntRect FUMeasureTextByTextRun(int x, int y, int width, const TextRun& textRun, int* descender, bool* pbNeedEllipsis);

void FUSetSkPaintByTextRun(SkPaint& skPaint, const TextRun& textRun);

#else

struct NVGcontext;

void FUSetNanoVGByTextRun(NVGcontext* ctx, const TextRun& textRun);

IntRect FUMeasureTextByTextRun(NVGcontext* ctx, int x, int y, int width, const TextRun& textRun, 
	int* descender, bool* pbNeedEllipsis);

#endif