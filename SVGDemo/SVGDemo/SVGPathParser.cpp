#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGPathParser.h"
#include "SVGPath.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>
#include "SVGGradientParser.h"

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGPathParser::parse(const std::string& line) const
{
    std::string d = extractAttr(line, "d");
    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);

    Color fill = Color(255, 0, 0, 0);
    std::string fillUrl;

   
    if (!fillStr.empty() && fillStr != "none") {
        if (SVGGradientParser::isFillGradientUrl(fillStr)) {
            fillUrl = fillStr;
           
        }
        else if (fillStr[0] == '#') {
            fill = applyOpacity(parseColor(fillStr), fillOpacity);
            
        }
        else {
            // fallback: đen đặc
            fill = Color(255, 0, 0, 0);
            
        }

    }
    
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* path = new SVGPath(std::wstring(d.begin(), d.end()), fill, stroke);
    path->setFillUrl(fillUrl);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        path->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    

    return path;
}
