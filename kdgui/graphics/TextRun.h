
#ifndef TextRun_h
#define TextRun_h

struct TextRun {
public:
	TextRun(const CStdString& c) 
		: characters(c)
		, bUnderline(false)
		, bOverflowEllipsis(false)
#ifdef _MSC_VER
		, align(SkPaint::kLeft_Align)
#else
		, align(eNRSTextAnchorStart)
#endif
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
#ifdef _MSC_VER
	SkPaint::Align align;
#else
	NRSTextAnchor align;
#endif
	
	SkScalar textSize;
	SkColor color;
};

#endif // TextRun_h