
#ifndef TextRun_h
#define TextRun_h

struct TextRun {
public:
	TextRun(const CStdString& c) 
		: characters(c)
		, bUnderline(false)
		, bOverflowEllipsis(false)
		, align(SkPaint::kLeft_Align)
		, color(0xFF000000)
		, familyName(_SC("Î¢ÈíÑÅºÚ"))
		, textSize(20)
		, bBold(false) {

	}

	bool bUnderline;
	bool bBold;
	bool bOverflowEllipsis;

	CStdString characters;
	CStdString familyName;
	SkPaint::Align align;
	SkScalar textSize;
	SkColor color;
};

#endif // TextRun_h