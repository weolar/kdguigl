/*
 * Copyright (C) 2002, 2003 The Karbon Developers
 * Copyright (C) 2006, 2007 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGParserUtilities_h
#define SVGParserUtilities_h
//#if ENABLE(SVG)

#include "ParserUtilities.h"
//#include <wtf/HashSet.h>

typedef std::pair<unsigned, unsigned> UnicodeRange;
typedef WTF::Vector<UnicodeRange> UnicodeRanges;

//namespace WebCore {

class SVGPointList;

bool parseNumber(const SQChar*& ptr, const SQChar* end, float& number, bool skip = true);
bool parseNumberOptionalNumber(const CStdString& s, float& h, float& v);
bool parseArcFlag(const SQChar*& ptr, const SQChar* end, bool& flag);

// SVG allows several different whitespace characters:
// http://www.w3.org/TR/SVG/paths.html#PathDataBNF
inline bool isWhitespace(const SQChar& c)
{
    return c == _SC(' ') || c == _SC('\t') || c == _SC('\n') || c == _SC('\r');
}

inline bool skipOptionalSpaces(const SQChar*& ptr, const SQChar* end)
{
    while (ptr < end && isWhitespace(*ptr))
        ptr++;
    return ptr < end;
}

inline bool skipOptionalSpacesOrDelimiter(const SQChar*& ptr, const SQChar* end, SQChar delimiter = ',')
{
    if (ptr < end && !isWhitespace(*ptr) && *ptr != delimiter)
        return false;
    if (skipOptionalSpaces(ptr, end)) {
        if (ptr < end && *ptr == delimiter) {
            ptr++;
            skipOptionalSpaces(ptr, end);
        }
    }
    return ptr < end;
}

// bool pointsListFromSVGData(SVGPointList& pointsList, const CStdString& points);
// WTF::Vector<CStdString> parseDelimitedString(const CStdString& input, const char seperator);
// bool parseKerningUnicodeString(const String& input, UnicodeRanges&, HashSet<String>& stringList);
// bool parseGlyphName(const CStdString& input, HashSet<String>& values);

//} // namespace WebCore

//#endif // ENABLE(SVG)
#endif // SVGParserUtilities_h
