#include <UIlib.h>
#include "FontUtil.h"
#include "Graphics/nanovg/nanovg.h"
#ifndef _MSC_VER
#include "dom/style/NodeStyle.h"
#endif
#include "Graphics/TextRun.h"

// 如果是居中、右对齐，外部传入textRun的时候已经做了调整
#ifdef _MSC_VER

IntRect FUMeasureTextByTextRun(int x, int y, int width, const TextRun& textRun, int* descender, bool* pbNeedEllipsis) {
	KDASSERT(!(SkPaint::kLeft_Align != textRun.align && 0 == width) && !(textRun.bOverflowEllipsis && 0 == width));

	SkPaint skPaint;
	FUSetSkPaintByTextRun(skPaint, textRun);

	SkRect bounds;
#ifdef _MSC_VER
	CStringA strUtf8(CW2A(textRun.characters, CP_UTF8));
#else
	CString strUtf8(textRun.characters);
#endif

	skPaint.measureText((LPCSTR)strUtf8, strUtf8.GetLength(), &bounds);

	SkPaint::FontMetrics metrics = {0};
	skPaint.getFontMetrics(&metrics, 0);

	*descender = SkScalarTruncToInt(round(metrics.fDescent));

	IntRect result(bounds);
	if (0 == width)
		width = result.width();

	result.move(-result.x(), -result.y());
	result.setHeight(result.height() + *descender);

	if (textRun.bOverflowEllipsis && result.width() > width) {
		*pbNeedEllipsis = true;
		result.setWidth(width);
	}

	if (SkPaint::kCenter_Align == textRun.align) {
		result.move((width - result.width())/2, 0);
	} else if (SkPaint::kRight_Align == textRun.align) {
		result.move(width - result.width(), 0);
	}

	result.move(x, y - result.height());

	return result;
}

void FUSetSkPaintByTextRun(SkPaint& skPaint, const TextRun& textRun) {
	skPaint.setTextEncoding(SkPaint::kUTF8_TextEncoding);
	skPaint.setTextSize(textRun.textSize);
	skPaint.setColor(textRun.color);
	skPaint.setAntiAlias(true);

	uint32_t nFlag = 0;
	if (_SC("宋体") == textRun.familyName || _SC("微软雅黑") == textRun.familyName)
		nFlag = SkPaint::kGenA8FromLCD_Flag;
	skPaint.setFlags(nFlag | SkPaint::kAntiAlias_Flag);

	if (textRun.bUnderline)
		skPaint.setUnderlineText(true);

	SkTypeface::Style style = !textRun.bBold ? SkTypeface::kNormal : SkTypeface::kBold;
	if (_SC("tahoma") == textRun.familyName && -1 != textRun.characters.Find(_SC("℃"))) // 针对kdugi2做个补丁，能在Tahoma下显示一些unicode字形
		SkSafeUnref(skPaint.setTypeface(SkTypeface::CreateFromNameA(_SC("宋体"), style)));
	else
		SkSafeUnref(skPaint.setTypeface(SkTypeface::CreateFromNameA(textRun.familyName, style)));

	skPaint.setTextAlign(textRun.align);
}

#else

void FUSetNanoVGByTextRun(NVGcontext* ctx, const TextRun& textRun) {
	//LOGI("FUSetNanoVGByTextRun: %f", textRun.textSize);
	nvgFontSize(ctx, textRun.textSize);

	if (eNRSTextAnchorStart == textRun.align)
		nvgTextAlign(ctx, NVG_ALIGN_LEFT);
	else if (eNRSTextAnchorMiddle == textRun.align)
		nvgTextAlign(ctx, NVG_ALIGN_CENTER);
	else if (eNRSTextAnchorEnd == textRun.align)
		nvgTextAlign(ctx, NVG_ALIGN_RIGHT);

	//nvgFontFace(ctx, textRun.familyName.GetString());
}

IntRect FUMeasureTextByTextRun(NVGcontext* ctx, int x, int y, int width, const TextRun& textRun, 
	int* descender, bool* pbNeedEllipsis) {
	FUSetNanoVGByTextRun(ctx, textRun);

	float bounds[4] = {0};
	nvgTextBounds(ctx, x, y, textRun.characters.GetString(), NULL, bounds);

	return IntRect((int)bounds[0], (int)bounds[1], (int)bounds[2], (int)bounds[3]);
}

#endif