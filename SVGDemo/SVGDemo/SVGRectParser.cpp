#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGRectParser.h"
#include "SVGRect.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGRectParser::parse(const std::string& line) const{

    float x = safeParseFloat(extractAttr(line, "x"), 0.0f);
    float y = safeParseFloat(extractAttr(line, "y"), 0.0f);
    float width = safeParseFloat(extractAttr(line, "width"), 0.0f);
    float height = safeParseFloat(extractAttr(line, "height"), 0.0f);
    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    svg::Point center = { x + width / 2.0f, y + height / 2.0f };
    auto* rect = new SVGRect(center, width, height, fill, stroke, strokeWidth);

    std::string transformStr = extractAttr(line, "transform");

    if (!transformStr.empty())
    {
        rect->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return rect;
}
