#ifndef SVGPathParserFactory_h
#define SVGPathParserFactory_h

class KdPath;

class SVGPathParserFactory {
public:
	//static SVGPathParserFactory* self();

	// String/SVGPathByteStream -> Path
	bool buildPathFromString(const CStdString& d, KdPath& result);
};

#endif // SVGPathParserFactory_h