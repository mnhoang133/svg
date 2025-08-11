#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGPolylineParser.h"
#include "SVGPolyline.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGPolylineParser::parse(const std::string& line) const 
{
    std::string rawPoints = extractAttr(line, "points");

    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillStr = extractAttr(line, "fill");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");
    std::string fillOpStr = extractAttr(line, "fill-opacity");

    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);
    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);

    float strokeWidth = safeParseFloat(extractAttr(line, "stroke-width"), 1.0f);

    bool strokeInvisible = (strokeStr.empty() || strokeStr == "none" || strokeOpacity == 0.0f);
    bool fillInvisible = (fillStr.empty() || fillStr == "none" || fillOpacity == 0.0f);

    Color stroke, fill;
    if (strokeInvisible && fillInvisible) {
        stroke = applyOpacity(Color(70, 70, 70), 1.0f);
        fill = applyOpacity(Color(110, 110, 110), 0.9f);
    }
    else {
        if (strokeStr.empty() || strokeStr == "none")
            stroke = Color(0, 0, 0, 0);
        else
            stroke = applyOpacity(parseColor(strokeStr), strokeOpacity);

        if (fillStr.empty() || fillStr == "none")
            fill = Color(0, 0, 0, 0);
        else
            fill = applyOpacity(parseColor(fillStr), fillOpacity);
    }

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

    auto* polyline = new SVGPolyline(points, stroke, strokeWidth, fill);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        polyline->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return polyline;
}
