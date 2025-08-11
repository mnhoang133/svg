#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SVGGParser.h"
#include "SVGCircleParser.h"
#include "SVGEllipseParser.h"
#include "SVGLineParser.h"
#include "SVGRectParser.h"
#include "SVGPolylineParser.h"
#include "SVGPathParser.h"
#include "SVGTextParser.h"
#include "SVGPolygonParser.h"
#include "SVGG.h"
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <cstdlib>

using namespace ParserUtils;
using namespace AttributeParserUtils;
using namespace Gdiplus;

SVGElement* SVGGParser::parse(const std::string& block) const
{
    auto* g = new SVGG();

    std::string openTag = extractOpeningTag(block);
    std::string innerContent = extractInnerContent(block);

    std::string styleStr = extractAttr(openTag, "style");
    if (!styleStr.empty()) {
        Gdiplus::Color stroke = parseStyleColor(styleStr, "stroke", true);
        float strokeWidth = parseStyleFloat(styleStr, "stroke-width", 1.0f);
        float strokeOpacity = parseStyleFloat(styleStr, "stroke-opacity", 1.0f);
        Gdiplus::Color fill = parseStyleColor(styleStr, "fill", false);
        float fillOpacity = parseStyleFloat(styleStr, "fill-opacity", 1.0f);
        g->setStyle(stroke, strokeWidth, strokeOpacity, fill, fillOpacity);
    }

    std::string transformStr = extractAttr(openTag, "transform");
    if (!transformStr.empty()) {
        g->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }

    auto children = extractChildElements(innerContent);

    int childCount = 0;
    for (const auto& child : children) {

        SVGElement* elem = nullptr;
        if (child.find("<g") != std::string::npos)
        {
            SVGGParser GP;
            elem = GP.parse(child);
        }
        else {
            std::string merged = mergeAttributes(openTag, child);

            if (extractAttr(merged, "transform").empty()) {
                std::string parentTransform = extractAttr(openTag, "transform");
                if (!parentTransform.empty()) {
                    // Gắn thủ công
                    size_t insertPos = merged.find('>');
                    if (insertPos != std::string::npos && merged[insertPos - 1] == '/') {
                        // xử lý tag tự đóng: <circle ... />
                        merged.insert(insertPos - 1, " transform=\"" + parentTransform + "\"");
                    }
                    else {
                        merged.insert(insertPos, " transform=\"" + parentTransform + "\"");
                    }
                }
            }


            if (merged.find("<circle") != std::string::npos)
            {
                SVGCircleParser PCircle;
                elem = PCircle.parse(merged);
            }
            else if (merged.find("<rect") != std::string::npos)
            {
                SVGRectParser PRect;
                elem = PRect.parse(merged);
            }
            else if (merged.find("<ellipse") != std::string::npos)
            {
                SVGEllipseParser PEllipse;
                elem = PEllipse.parse(merged);
            }
            else if (merged.find("<line") != std::string::npos)
            {
                SVGLineParser PLine;
                elem = PLine.parse(merged);
            }
            else if (merged.find("<text") != std::string::npos)
            {
                SVGTextParser PText;
                elem = PText.parse(merged);
            }
            else if (merged.find("<path") != std::string::npos)
            {
                SVGPathParser PPath;
                elem = PPath.parse(merged);
            }
            else if (merged.find("<polyline") != std::string::npos)
            {
                SVGPolylineParser PPolyline;
                elem = PPolyline.parse(merged);
            }
            else if (merged.find("<polygon") != std::string::npos)
            {
                SVGPolygonParser PPolygon;
                elem = PPolygon.parse(merged);
            }
        }
        if (elem) {
            g->addChild(elem);
            childCount++;
        }
    }

    return g;
}
