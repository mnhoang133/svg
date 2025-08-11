#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGCircleParser.h"
#include "SVGCircle.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGCircleParser::parse(const std::string& line) const
{
    float cx = safeParseFloat(extractAttr(line, "cx"), 0.0f);
    float cy = safeParseFloat(extractAttr(line, "cy"), 0.0f);
    float r = safeParseFloat(extractAttr(line, "r"), 0.0f);
    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* circle = new SVGCircle({ cx, cy }, r, fill, stroke, strokeWidth);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        circle->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return circle;
}
