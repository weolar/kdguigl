
#include <UIlib.h>
#include "SVGPathParserFactory.h"
#include "SVGPathBuilder.h"
#include "SVGPathParser.h"
#include "SVGPathStringSource.h"

bool SVGPathParserFactory::buildPathFromString(const CStdString& d, KdPath& result) {
	if (d.IsEmpty())
		return false;

	SVGPathBuilder* builder = new SVGPathBuilder();
	builder->setCurrentPath(&result);

	//OwnPtr<SVGPathStringSource> source = SVGPathStringSource::create(d);
	SVGPathStringSource* source = new SVGPathStringSource(d);
	//SVGPathParser* parser = globalSVGPathParser(source.get(), builder);
	SVGPathParser* parser = new SVGPathParser();
	parser->setCurrentSource(source);
	parser->setCurrentConsumer(builder);

	bool ok = parser->parsePathDataFromSource(NormalizedParsing);
	parser->cleanup();

	delete parser;
	delete source;
	delete builder;

	return ok;
}