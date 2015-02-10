/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef KdColor_h
#define KdColor_h

// #include <wtf/FastAllocBase.h>
// #include <wtf/Forward.h>
// #include <wtf/unicode/Unicode.h>

//namespace WebCore {

class KdColor;

typedef unsigned RGBA32;        // RGBA quadruplet

RGBA32 makeRGB(int r, int g, int b);
RGBA32 makeRGBA(int r, int g, int b, int a);

RGBA32 colorWithOverrideAlpha(RGBA32 color, float overrideAlpha);
RGBA32 makeRGBA32FromFloats(float r, float g, float b, float a);
RGBA32 makeRGBAFromHSLA(double h, double s, double l, double a);
RGBA32 makeRGBAFromCMYKA(float c, float m, float y, float k, float a);

int differenceSquared(const KdColor&, const KdColor&);

inline int redChannel(RGBA32 color) { return (color >> 16) & 0xFF; }
inline int greenChannel(RGBA32 color) { return (color >> 8) & 0xFF; }
inline int blueChannel(RGBA32 color) { return color & 0xFF; }
inline int alphaChannel(RGBA32 color) { return (color >> 24) & 0xFF; }

class KdColor {
    WTF_MAKE_FAST_ALLOCATED;
public:
    KdColor() : m_color(0), m_valid(false) { }
    KdColor(RGBA32 col) : m_color(col), m_valid(true) { }
    KdColor(int r, int g, int b) : m_color(makeRGB(r, g, b)), m_valid(true) { }
    KdColor(int r, int g, int b, int a) : m_color(makeRGBA(r, g, b, a)), m_valid(true) { }
    // KdColor is currently limited to 32bit RGBA, perhaps some day we'll support better colors
    KdColor(float r, float g, float b, float a) : m_color(makeRGBA32FromFloats(r, g, b, a)), m_valid(true) { }
    // Creates a new color from the specific CMYK and alpha values.
    KdColor(float c, float m, float y, float k, float a) : m_color(makeRGBAFromCMYKA(c, m, y, k, a)), m_valid(true) { }
    //explicit KdColor(const String&);
    explicit KdColor(const char*);

//     // Returns the color serialized according to HTML5
//     // - http://www.whatwg.org/specs/web-apps/current-work/#serialization-of-a-color
//     String serialized() const;
// 
//     // Returns the color serialized as either #RRGGBB or #RRGGBBAA
//     // The latter format is not a valid CSS color, and should only be seen in DRT dumps.
//     String nameForRenderTreeAsText() const;

    //void setNamedColor(const String&);

    bool isValid() const { return m_valid; }

    bool hasAlpha() const { return alpha() < 255; }

    int red() const { return redChannel(m_color); }
    int green() const { return greenChannel(m_color); }
    int blue() const { return blueChannel(m_color); }
    int alpha() const { return alphaChannel(m_color); }
    
    RGBA32 rgb() const { return m_color; } // Preserve the alpha.
    void setRGB(int r, int g, int b) { m_color = makeRGB(r, g, b); m_valid = true; }
    void setRGB(RGBA32 rgb) { m_color = rgb; m_valid = true; }
    void getRGBA(float& r, float& g, float& b, float& a) const;
    void getRGBA(double& r, double& g, double& b, double& a) const;
    void getHSL(double& h, double& s, double& l) const;

    KdColor light() const;
    KdColor dark() const;

    // This is an implementation of Porter-Duff's "source-over" equation
    KdColor blend(const KdColor&) const;
    KdColor blendWithWhite() const;

//     static bool parseHexColor(const String& name, RGBA32& rgb);
//     static bool parseHexColor(const UChar* name, unsigned length, RGBA32& rgb);

    static const RGBA32 black = 0xFF000000;
    static const RGBA32 white = 0xFFFFFFFF;
    static const RGBA32 darkGray = 0xFF808080;
    static const RGBA32 gray = 0xFFA0A0A0;
    static const RGBA32 lightGray = 0xFFC0C0C0;
    static const RGBA32 transparent = 0x00000000;

private:
    RGBA32 m_color;
    bool m_valid;
};

inline bool operator==(const KdColor& a, const KdColor& b)
{
    return a.rgb() == b.rgb() && a.isValid() == b.isValid();
}

inline bool operator!=(const KdColor& a, const KdColor& b)
{
    return !(a == b);
}

KdColor colorFromPremultipliedARGB(unsigned);
unsigned premultipliedARGBFromColor(const KdColor&);

//} // namespace WebCore

#endif // KdColor_h
