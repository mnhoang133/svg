#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGTextParser.h"
#include "SVGText.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGTextParser::parse(const std::string& line) const
{
    float x = safeParseFloat(extractAttr(line, "x"), 0.0f);
    float y = safeParseFloat(extractAttr(line, "y"), 0.0f);
    int fontSize = safeParseInt(extractAttr(line, "font-size"), 12);

    std::string fillStr = extractAttr(line, "fill");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);

    std::size_t start = line.find('>');
    std::size_t end = line.find('<', start);
    std::string content = (start != std::string::npos && end != std::string::npos) ? line.substr(start + 1, end - start - 1) : "";

    std::wstring wcontent(content.begin(), content.end());
    auto* text = new SVGText({ x, y }, wcontent, fontSize, fill);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        text->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return text;
}
