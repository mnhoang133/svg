#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGPolygonParser.h"
#include "SVGPolygon.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGPolygonParser::parse(const std::string& line) const
{
    std::string rawPoints = extractAttr(line, "points");
    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillStr = extractAttr(line, "fill");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");
    std::string fillOpStr = extractAttr(line, "fill-opacity");

    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);
    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);

    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);
    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);

    std::vector<svg::Point> points;
    std::istringstream iss(rawPoints);
    std::string pair;
    while (iss >> pair) {
        size_t comma = pair.find(',');
        if (comma != std::string::npos) {
            float x = safeParseFloat(pair.substr(0, comma), 0.0f);
            float y = safeParseFloat(pair.substr(comma + 1), 0.0f);
            points.push_back({ x, y });
        }
    }

    auto* polygon = new SVGPolygon(points, fill, stroke, strokeWidth);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        polygon->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return polygon;
}
