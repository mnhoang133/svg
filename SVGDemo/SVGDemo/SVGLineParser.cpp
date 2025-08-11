#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGLineParser.h"
#include "SVGLine.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGLineParser::parse(const std::string& line) const 
{
    float x1 = safeParseFloat(extractAttr(line, "x1"), 0.0f);
    float y1 = safeParseFloat(extractAttr(line, "y1"), 0.0f);
    float x2 = safeParseFloat(extractAttr(line, "x2"), 0.0f);
    float y2 = safeParseFloat(extractAttr(line, "y2"), 0.0f);
    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    std::string strokeStr = extractAttr(line, "stroke");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* svgline = new SVGLine({ x1, y1 }, { x2, y2 }, stroke, strokeWidth);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        svgline->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return svgline;
}
