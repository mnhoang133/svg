#include "stdafx.h"
#include "SVGGradientParser.h"
#include "SVGAttributeUtils.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"
#include "GradientManager.h"

#include "ParserUtils.h"
#include <regex>
#include <sstream>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace AttributeParserUtils;
using namespace ParserUtils;

void SVGGradientParser::parseDefs(const std::string& defsContent) {
    // Regex để tìm tất cả linearGradient tags
    std::regex linearGradientRegex(R"(<linearGradient[\s\S]*?</linearGradient>)");
    auto linearBegin = std::sregex_iterator(defsContent.begin(), defsContent.end(), linearGradientRegex);
    auto linearEnd = std::sregex_iterator();

    for (auto it = linearBegin; it != linearEnd; ++it) {
        std::string gradientTag = it->str();
        SVGLinearGradient* gradient = parseLinearGradient(gradientTag);
        if (gradient && !gradient->id.empty()) {
            GradientManager::addGradient(gradient->id, gradient);
        }
    }

    // Regex để tìm tất cả radialGradient tags
    std::regex radialGradientRegex(R"(<radialGradient[\s\S]*?</radialGradient>)");
    auto radialBegin = std::sregex_iterator(defsContent.begin(), defsContent.end(), radialGradientRegex);
    auto radialEnd = std::sregex_iterator();

    for (auto it = radialBegin; it != radialEnd; ++it) {
        std::string gradientTag = it->str();
        SVGRadialGradient* gradient = parseRadialGradient(gradientTag);
        if (gradient && !gradient->id.empty()) {
            GradientManager::addGradient(gradient->id, gradient);
        }
    }
}

SVGLinearGradient* SVGGradientParser::parseLinearGradient(const std::string& gradientTag) {
    SVGLinearGradient* gradient = new SVGLinearGradient();

    // Extract opening tag để lấy attributes
    size_t firstClose = gradientTag.find('>');
    if (firstClose == std::string::npos) {
        delete gradient;
        return nullptr;
    }

    std::string openingTag = gradientTag.substr(0, firstClose + 1);
    std::string content = gradientTag.substr(firstClose + 1);

    // Parse attributes
    gradient->id = extractAttr(openingTag, "id");
    gradient->gradientUnits = extractAttr(openingTag, "gradientUnits");
    if (gradient->gradientUnits.empty()) {
        gradient->gradientUnits = "objectBoundingBox";
    }

    gradient->href = extractAttr(openingTag, "href");
    if (gradient->href.empty()) {
        gradient->href = extractAttr(openingTag, "xlink:href");
    }

    // Parse coordinates
    std::string x1Str = extractAttr(openingTag, "x1");
    std::string y1Str = extractAttr(openingTag, "y1");
    std::string x2Str = extractAttr(openingTag, "x2");
    std::string y2Str = extractAttr(openingTag, "y2");

    gradient->x1 = parseCoordinate(x1Str, 0.0f);
    gradient->y1 = parseCoordinate(y1Str, 0.0f);
    gradient->x2 = parseCoordinate(x2Str, 1.0f);
    gradient->y2 = parseCoordinate(y2Str, 0.0f);

    // Parse stops
    parseGradientStops(content, gradient);

    return gradient;
}

SVGRadialGradient* SVGGradientParser::parseRadialGradient(const std::string& gradientTag) {
    SVGRadialGradient* gradient = new SVGRadialGradient();

    // Extract opening tag để lấy attributes
    size_t firstClose = gradientTag.find('>');
    if (firstClose == std::string::npos) {
        delete gradient;
        return nullptr;
    }

    std::string openingTag = gradientTag.substr(0, firstClose + 1);
    std::string content = gradientTag.substr(firstClose + 1);

    // Parse attributes
    gradient->id = extractAttr(openingTag, "id");
    gradient->gradientUnits = extractAttr(openingTag, "gradientUnits");
    if (gradient->gradientUnits.empty()) {
        gradient->gradientUnits = "objectBoundingBox";
    }

    gradient->href = extractAttr(openingTag, "href");
    if (gradient->href.empty()) {
        gradient->href = extractAttr(openingTag, "xlink:href");
    }

    // Parse coordinates
    std::string cxStr = extractAttr(openingTag, "cx");
    std::string cyStr = extractAttr(openingTag, "cy");
    std::string rStr = extractAttr(openingTag, "r");
    std::string fxStr = extractAttr(openingTag, "fx");
    std::string fyStr = extractAttr(openingTag, "fy");

    gradient->cx = parseCoordinate(cxStr, 0.5f);
    gradient->cy = parseCoordinate(cyStr, 0.5f);
    gradient->r = parseCoordinate(rStr, 0.5f);
    gradient->fx = parseCoordinate(fxStr, gradient->cx);
    gradient->fy = parseCoordinate(fyStr, gradient->cy);

    // Parse stops
    parseGradientStops(content, gradient);

    return gradient;
}

void SVGGradientParser::parseGradientStops(const std::string& gradientContent, SVGGradient* gradient) {
    if (!gradient) return;

    // Regex để tìm tất cả stop tags
    std::regex stopRegex(R"(<stop[^>]*\/?>)");
    auto stopsBegin = std::sregex_iterator(gradientContent.begin(), gradientContent.end(), stopRegex);
    auto stopsEnd = std::sregex_iterator();

    for (auto it = stopsBegin; it != stopsEnd; ++it) {
        std::string stopTag = it->str();
        GradientStop stop = parseStop(stopTag);
        gradient->addStop(stop);
    }
}

GradientStop SVGGradientParser::parseStop(const std::string& stopTag) {
    GradientStop stop;

    // Parse offset
    std::string offsetStr = extractAttr(stopTag, "offset");
    if (!offsetStr.empty()) {
        // Remove % if present
        if (offsetStr.back() == '%') {
            offsetStr.pop_back();
            stop.offset = safeParseFloat(offsetStr, 0.0f) / 100.0f;
        }
        else {
            stop.offset = safeParseFloat(offsetStr, 0.0f);
        }
    }

    // Parse stop-color
    std::string stopColor = extractAttr(stopTag, "stop-color");
    if (!stopColor.empty()) {
        stop.color = parseColor(stopColor);
    }

    // Parse stop-opacity
    stop.opacity = parseStopOpacity(stopTag);

    // Check style attribute for stop-color and stop-opacity
    std::string styleStr = extractAttr(stopTag, "style");
    if (!styleStr.empty()) {
        // Parse stop-color from style
        std::regex stopColorRegex(R"(stop-color\s*:\s*([^;]+))");
        std::smatch match;
        if (std::regex_search(styleStr, match, stopColorRegex)) {
            stop.color = parseColor(match[1]);
        }

        // Parse stop-opacity from style
        std::regex stopOpacityRegex(R"(stop-opacity\s*:\s*([^;]+))");
        if (std::regex_search(styleStr, match, stopOpacityRegex)) {
            stop.opacity = safeParseFloat(match[1], 1.0f);
        }
    }

    // Clamp offset to [0, 1]
    if (stop.offset < 0.0f) stop.offset = 0.0f;
    if (stop.offset > 1.0f) stop.offset = 1.0f;
    if (stop.opacity < 0.0f) stop.opacity = 0.0f;
    if (stop.opacity > 1.0f) stop.opacity = 1.0f;

    return stop;
}

bool SVGGradientParser::isFillGradientUrl(const std::string& fillValue) {
    return fillValue.find("url(#") == 0 && fillValue.back() == ')';
}

std::string SVGGradientParser::extractGradientId(const std::string& url) {
    if (url.find("url(#") == 0 && url.back() == ')') {
        return url.substr(5, url.length() - 6);
    }
    return "";
}

float SVGGradientParser::parseCoordinate(const std::string& value, float defaultVal) {
    if (value.empty()) return defaultVal;

    std::string val = value;
    // Remove % if present and convert to decimal
    if (val.back() == '%') {
        val.pop_back();
        return safeParseFloat(val, defaultVal * 100.0f) / 100.0f;
    }

    return safeParseFloat(val, defaultVal);
}

float SVGGradientParser::parseStopOpacity(const std::string& stopTag) {
    std::string opacityStr = extractAttr(stopTag, "stop-opacity");
    return safeParseFloat(opacityStr, 1.0f);
}