#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGEllipseParser.h"
#include "SVGEllipse.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGEllipseParser::parse(const std::string& line) const 
{
    float cx = safeParseFloat(extractAttr(line, "cx"), 0.0f);
    float cy = safeParseFloat(extractAttr(line, "cy"), 0.0f);
    float rx = safeParseFloat(extractAttr(line, "rx"), 0.0f);
    float ry = safeParseFloat(extractAttr(line, "ry"), 0.0f);
    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* ellipse = new SVGEllipse({ cx, cy }, rx, ry, fill, stroke, strokeWidth);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        ellipse->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return ellipse;
}
