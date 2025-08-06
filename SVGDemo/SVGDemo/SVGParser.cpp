#include "stdafx.h"
#include "SVGParser.h"
#include "Point.h"

#include <regex>
#include <algorithm>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <gdiplus.h>

using namespace Gdiplus;

// ====== Helper: Safe parse float/int =======
float safeParseFloat(const std::string& str, float defaultValue = 0.0f) {
    try {
        if (str.empty()) return defaultValue;
        return std::stof(str);
    }
    catch (const std::exception&) {
        return defaultValue;
    }
}

int safeParseInt(const std::string& str, int defaultValue = 0) {
    try {
        if (str.empty()) return defaultValue;
        return std::stoi(str);
    }
    catch (const std::exception&) {
        return defaultValue;
    }
}

// Ham ap dung do mo cho mau
Gdiplus::Color applyOpacity(const Gdiplus::Color& color, float opacity) {
    int alpha = static_cast<int>(opacity * 255.0f);
    return Gdiplus::Color(alpha, color.GetRed(), color.GetGreen(), color.GetBlue());
}

// Ham chuyen chuoi SVG mau thanh doi tuong Color
Color SVGParser::parseColor(const std::string& s) {
    std::string str = s;
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (str.empty() || str == "none")
        return Color(0, 0, 0, 0); // mau trong suot

    if (str[0] == '#') {
        unsigned int hex = 0;
        try {
            hex = std::stoul(str.substr(1), nullptr, 16);
        }
        catch (const std::exception&) {
            return Color(255, 0, 0, 0);
        }
        return Color(255, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }

    if (str.find("rgb(") == 0) {
        size_t start = str.find("(");
        size_t end = str.find(")");
        if (start == std::string::npos || end == std::string::npos || end <= start)
            return Color(255, 0, 0, 0);

        std::string rgbContent = str.substr(start + 1, end - start - 1);
        rgbContent.erase(remove_if(rgbContent.begin(), rgbContent.end(), ::isspace), rgbContent.end());

        std::istringstream iss(rgbContent);
        std::string token;
        int r = 0, g = 0, b = 0;

        if (std::getline(iss, token, ',')) r = safeParseInt(token, 0);
        if (std::getline(iss, token, ',')) g = safeParseInt(token, 0);
        if (std::getline(iss, token, ',')) b = safeParseInt(token, 0);

        return Color(255, r, g, b);
    }

    return Color(255, 0, 0, 0);
}

// Ham trich thuoc tinh cua the SVG tu chuoi
std::string SVGParser::extractAttr(const std::string& tag, const std::string& attr) {
    std::string search = attr + "=";
    size_t pos = tag.find(search);
    while (pos != std::string::npos) {
        if (pos == 0 || isspace(tag[pos - 1])) {
            size_t start = pos + search.length();
            if (start >= tag.length()) return "";
            char quote = tag[start];
            if (quote != '"' && quote != '\'') return "";
            ++start;
            size_t end = tag.find(quote, start);
            if (end == std::string::npos) return "";
            return tag.substr(start, end - start);
        }
        pos = tag.find(search, pos + 1);
    }
    return "";
}

Gdiplus::Color SVGParser::parseStyleColor(const std::string& styleStr, const std::string& key, bool isStroke) {
    std::regex regex(key + R"(\s*:\s*([^;]+))");
    std::smatch match;
    if (std::regex_search(styleStr, match, regex)) {
        std::string colorStr = match[1];
        return parseColor(colorStr);
    }

    // Nếu fill thì default là black, nếu stroke thì default là transparent
    return isStroke ? Gdiplus::Color(0, 0, 0, 0) : Gdiplus::Color(0, 0, 0);
}

float SVGParser::parseStyleFloat(const std::string& styleStr, const std::string& key, float defaultValue) {
    std::regex regex(key + R"(\s*:\s*([^;]+))");
    std::smatch match;
    if (std::regex_search(styleStr, match, regex)) {
        return safeParseFloat(match[1], defaultValue);
    }
    return defaultValue;
}

std::string extractOpeningTag(const std::string& block) {
    size_t start = block.find('<');
    size_t end = block.find('>');
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return "";
    return block.substr(start, end - start + 1);
}

std::string extractInnerContent(const std::string& block) {
    size_t start = block.find('>');
    size_t end = block.rfind("</g>");
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return "";
    return block.substr(start + 1, end - start - 1);
}

std::vector<std::string> extractChildElements(const std::string& innerContent) {
    std::vector<std::string> result;
    size_t pos = 0;
    while (pos < innerContent.size()) {
        // Tìm tag mở
        size_t tagStart = innerContent.find('<', pos);
        if (tagStart == std::string::npos) break;

        // Lấy tên tag
        size_t tagNameStart = tagStart + 1;
        while (tagNameStart < innerContent.size() && isspace(innerContent[tagNameStart])) tagNameStart++;
        size_t tagNameEnd = tagNameStart;
        while (tagNameEnd < innerContent.size() && isalpha(innerContent[tagNameEnd])) tagNameEnd++;
        std::string tagName = innerContent.substr(tagNameStart, tagNameEnd - tagNameStart);
        if (tagName.empty()) break;

        // Tìm tag đóng hoặc tự đóng
        size_t tagEnd = innerContent.find('>', tagNameEnd);
        if (tagEnd == std::string::npos) break;

        // Kiểm tra tự đóng
        bool isSelfClose = (innerContent[tagEnd - 1] == '/');
        if (isSelfClose) {
            // Thẻ tự đóng <rect .../> <circle .../> ...
            result.push_back(innerContent.substr(tagStart, tagEnd - tagStart + 1));
            pos = tagEnd + 1;
        }
        else {
            // Thẻ <g> ... </g> hoặc <rect>...</rect>
            std::string closeTag = "</" + tagName + ">";
            size_t closeTagPos = innerContent.find(closeTag, tagEnd);
            if (closeTagPos == std::string::npos) break;
            result.push_back(innerContent.substr(tagStart, closeTagPos + closeTag.size() - tagStart));
            pos = closeTagPos + closeTag.size();
        }
    }

    return result;
}

std::string readFullGBlock(std::ifstream& file, const std::string& firstLine) {
    std::string block = firstLine + "\n";
    int depth = 1;
    std::string line;
    while (depth > 0 && std::getline(file, line)) {
        block += line + "\n";
        if (line.find("<g") != std::string::npos) depth++;
        if (line.find("</g>") != std::string::npos) depth--;
    }
    return block;
}

std::string SVGParser::mergeAttributes(const std::string& parentTag, const std::string& childTag) {
    std::string merged = childTag;

    std::vector<std::string> attrs = {
        "stroke", "stroke-width", "stroke-opacity",
        "fill", "fill-opacity", "transform"
    };

    for (const std::string& attr : attrs) {
        if (extractAttr(childTag, attr).empty()) {
            std::string val = extractAttr(parentTag, attr);
            if (!val.empty()) {
                size_t pos = merged.find(' ');
                if (pos != std::string::npos) {
                    merged.insert(pos + 1, attr + "=\"" + val + "\" ");
                }
            }
        }
    }
    return merged;
}

// ================= SHAPES =====================

// Parse the <rect> tag thanh doi tuong SVGRect
SVGElement* SVGParser::parseRect(const std::string& line) {
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

// Parse the <circle> tag thanh doi tuong SVGCircle
SVGElement* SVGParser::parseCircle(const std::string& line) {
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

// Parse the <ellipse> tag thanh doi tuong SVGEllipse
SVGElement* SVGParser::parseEllipse(const std::string& line) {
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

// Parse the <line> tag thanh doi tuong SVGLine
SVGElement* SVGParser::parseLine(const std::string& line) {
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

// Parse the <polyline> tag thanh doi tuong SVGPolyline
SVGElement* SVGParser::parsePolyline(const std::string& line) {
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

// Parse the <polygon> tag thanh SVGPolygon
SVGElement* SVGParser::parsePolygon(const std::string& line) {
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

// Parse the <text> tag thanh SVGText
SVGElement* SVGParser::parseText(const std::string& line) {
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

// Parse the <path> tag thanh SVGPath
SVGElement* SVGParser::parsePath(const std::string& line) {
    std::string d = extractAttr(line, "d");
    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = safeParseFloat(fillOpStr, 1.0f);
    float strokeOpacity = safeParseFloat(strokeOpStr, 1.0f);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* path = new SVGPath(std::wstring(d.begin(), d.end()), fill, stroke);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        path->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return path;
}

SVGElement* SVGParser::parseG(const std::string& block) {
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


    for (const auto& child : children) {
        SVGElement* elem = nullptr;
        if (child.find("<g") != std::string::npos)
            elem = parseG(child);
        else {
            std::string merged = mergeAttributes(openTag, child);

            if (merged.find("<circle") != std::string::npos)
                elem = parseCircle(merged);
            else if (merged.find("<rect") != std::string::npos)
                elem = parseRect(merged);
            else if (merged.find("<ellipse") != std::string::npos)
                elem = parseEllipse(merged);
            else if (merged.find("<line") != std::string::npos)
                elem = parseLine(merged);
            else if (merged.find("<text") != std::string::npos)
                elem = parseText(merged);
            else if (merged.find("<path") != std::string::npos)
                elem = parsePath(merged);
            else if (merged.find("<polyline") != std::string::npos)
                elem = parsePolyline(merged);
            else if (merged.find("<polygon") != std::string::npos)
                elem = parsePolygon(merged);
        }
        if (elem) g->addChild(elem);
    }
    return g;
}

// Doc file SVG tu ten file va parse cac shape vao mot SVGGroup
SVGGroup* SVGParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;

    // Đọc toàn bộ file vào một string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string svgContent = buffer.str();

    // Xóa BOM nếu có
    if (svgContent.size() >= 3 &&
        (unsigned char)svgContent[0] == 0xEF &&
        (unsigned char)svgContent[1] == 0xBB &&
        (unsigned char)svgContent[2] == 0xBF)
        svgContent = svgContent.substr(3);

    SVGGroup* group = new SVGGroup({});

    // Regex tách từng shape chính, dùng non-greedy để tách đúng từng thẻ
    std::regex tagRegex(R"((<rect[\s\S]*?\/?>)|(<circle[\s\S]*?\/?>)|(<ellipse[\s\S]*?\/?>)|(<line[\s\S]*?\/?>)|(<text[\s\S]*?<\/text>)|(<path[\s\S]*?\/?>)|(<polyline[\s\S]*?\/?>)|(<polygon[\s\S]*?\/?>)|(<g[\s\S]*?<\/g>))");
    auto tagsBegin = std::sregex_iterator(svgContent.begin(), svgContent.end(), tagRegex);
    auto tagsEnd = std::sregex_iterator();

    for (auto it = tagsBegin; it != tagsEnd; ++it) {


        std::string line = it->str();
        if (line.find("<circle") != std::string::npos)
            group->addElement(parseCircle(line));
        else if (line.find("<rect") != std::string::npos)
            group->addElement(parseRect(line));
        else if (line.find("<ellipse") != std::string::npos)
            group->addElement(parseEllipse(line));
        else if (line.find("<line") != std::string::npos)
            group->addElement(parseLine(line));
        else if (line.find("<text") != std::string::npos)
            group->addElement(parseText(line));
        else if (line.find("<path") != std::string::npos)
            group->addElement(parsePath(line));
        else if (line.find("<polyline") != std::string::npos)
            group->addElement(parsePolyline(line));
        else if (line.find("<polygon") != std::string::npos)
            group->addElement(parsePolygon(line));
        else if (line.find("<g") != std::string::npos)
            group->addElement(parseG(line));
    }

    return group;
}
