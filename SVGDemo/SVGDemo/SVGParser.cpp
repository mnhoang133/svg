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
        unsigned int hex = std::stoul(str.substr(1), nullptr, 16);
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

        if (std::getline(iss, token, ',')) r = std::stoi(token);
        if (std::getline(iss, token, ',')) g = std::stoi(token);
        if (std::getline(iss, token, ',')) b = std::stoi(token);

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
    return isStroke ? Gdiplus::Color(0, 0, 0, 0) : Gdiplus::Color(0, 0, 0);  // RGBA = (0,0,0,0) hoặc (0,0,0,255)
}

float SVGParser::parseStyleFloat(const std::string& styleStr, const std::string& key, float defaultValue) {
    std::regex regex(key + R"(\s*:\s*([^;]+))");
    std::smatch match;
    if (std::regex_search(styleStr, match, regex)) {
        try {
            return std::stof(match[1]);  //  dòng gây crash
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing float for key '" << key << "': " << e.what() << std::endl;
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string extractOpeningTag(const std::string& line) {
    std::smatch match;
    std::regex pattern(R"(<\s*([^\s>/]+)[^>]*>)"); // khớp với <g ...>
    if (std::regex_search(line, match, pattern)) {
        return match.str(0);
    }
    return "";
}

std::string extractInnerContent(const std::string& line) {
    size_t openEnd = line.find('>');
    size_t closeStart = line.rfind('<');
    if (openEnd != std::string::npos && closeStart != std::string::npos && closeStart > openEnd) {
        return line.substr(openEnd + 1, closeStart - openEnd - 1);
    }
    return "";
}



std::vector<std::string> extractChildElements(const std::string& content) {
    std::vector<std::string> children;
    size_t pos = 0;
    int depth = 0;
    size_t start = 0;
    bool inTag = false;

    while (pos < content.size()) {
        if (content[pos] == '<') {
            if (content.compare(pos, 2, "</") == 0) {
                depth--;
                if (depth == 0 && inTag) {
                    size_t end = content.find('>', pos);
                    if (end != std::string::npos) {
                        children.push_back(content.substr(start, end - start + 1));
                        inTag = false;
                        pos = end;
                    }
                }
            }
            else if (content.compare(pos, 2, "<g") == 0 || content.compare(pos, 1, "<") == 0) {
                if (depth == 0) {
                    start = pos;
                    inTag = true;
                }
                if (content[pos + content.substr(pos).find('>') - 1] != '/') // not self-closing
                    depth++;
            }
        }
        pos++;
    }

    return children;
}


std::string readFullGBlock(std::ifstream& file, std::string currentLine) {
    std::string content = currentLine + "\n";
    int openTags = 0;

    if (currentLine.find("<g") != std::string::npos) openTags++;
    if (currentLine.find("</g>") != std::string::npos) openTags--;

    std::string line;
    while (openTags > 0 && std::getline(file, line)) {
        content += line + "\n";
        if (line.find("<g") != std::string::npos) openTags++;
        if (line.find("</g>") != std::string::npos) openTags--;
    }

    return content;
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
    float x = std::stof(extractAttr(line, "x"));
    float y = std::stof(extractAttr(line, "y"));
    float width = std::stof(extractAttr(line, "width"));
    float height = std::stof(extractAttr(line, "height"));
    float strokeWidth = std::stof(extractAttr(line, "stroke-width"));

    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);

    svg::Point center = { x + width / 2.0f, y + height / 2.0f };
    auto* rect = new SVGRect(center, width, height, fill, stroke, strokeWidth);

    //transform
    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty())
    {
        rect->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }
    return rect;
}

// Parse the <circle> tag thanh doi tuong SVGCircle
SVGElement* SVGParser::parseCircle(const std::string& line) {

    float cx = std::stof(extractAttr(line, "cx"));
    float cy = std::stof(extractAttr(line, "cy"));
    float r = std::stof(extractAttr(line, "r"));
    float strokeWidth = std::stof(extractAttr(line, "stroke-width"));


    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);

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
    float cx = std::stof(extractAttr(line, "cx"));
    float cy = std::stof(extractAttr(line, "cy"));
    float rx = std::stof(extractAttr(line, "rx"));
    float ry = std::stof(extractAttr(line, "ry"));
    float strokeWidth = std::stof(extractAttr(line, "stroke-width"));

    std::string fillStr = extractAttr(line, "fill");
    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");

    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);

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
    float x1 = std::stof(extractAttr(line, "x1"));
    float y1 = std::stof(extractAttr(line, "y1"));
    float x2 = std::stof(extractAttr(line, "x2"));
    float y2 = std::stof(extractAttr(line, "y2"));
    float strokeWidth = std::stof(extractAttr(line, "stroke-width"));

    std::string strokeStr = extractAttr(line, "stroke");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);
    //tránh đặt là line để tránh trùng với line tham chiếu vào
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

    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);
    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);

    float strokeWidth = 1.0f;
    std::string sw = extractAttr(line, "stroke-width");
    if (!sw.empty()) strokeWidth = std::stof(sw);

    // DEBUG STROKE/FILL neu tat ca deu "none" hoac opacity = 0
    bool strokeInvisible = (strokeStr.empty() || strokeStr == "none" || strokeOpacity == 0.0f);
    bool fillInvisible = (fillStr.empty() || fillStr == "none" || fillOpacity == 0.0f);

    Color stroke, fill;

    if (strokeInvisible && fillInvisible) {
        stroke = applyOpacity(Color(70, 70, 70), 1.0f);    // Gan den
        fill = applyOpacity(Color(110, 110, 110), 0.9f);   // Xam dam
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
            float x = std::stof(pair.substr(0, comma));
            float y = std::stof(pair.substr(comma + 1));
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
    float strokeWidth = std::stof(extractAttr(line, "stroke-width"));

    std::string strokeStr = extractAttr(line, "stroke");
    std::string fillStr = extractAttr(line, "fill");
    std::string strokeOpStr = extractAttr(line, "stroke-opacity");
    std::string fillOpStr = extractAttr(line, "fill-opacity");

    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);
    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);

    // Ap dung mau voi do mo
    Color stroke = strokeStr.empty() || strokeStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(strokeStr), strokeOpacity);
    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);

    std::vector<svg::Point> points;
    std::istringstream iss(rawPoints);
    std::string pair;
    while (iss >> pair) {
        size_t comma = pair.find(',');
        if (comma != std::string::npos) {
            float x = std::stof(pair.substr(0, comma));
            float y = std::stof(pair.substr(comma + 1));
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
    float x = std::stof(extractAttr(line, "x"));
    float y = std::stof(extractAttr(line, "y"));
    int fontSize = std::stoi(extractAttr(line, "font-size"));

    std::string fillStr = extractAttr(line, "fill");
    std::string fillOpStr = extractAttr(line, "fill-opacity");
    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);

    Color fill = fillStr.empty() || fillStr == "none" ? Color(0, 0, 0, 0) : applyOpacity(parseColor(fillStr), fillOpacity);

    // Lay noi dung giua the <text>...</text>
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

    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);

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
            elem = parseG(child); // không merge
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
    auto* group = new SVGGroup({});
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << " Failed to open file: " << filename << '\n';
        return group;
    }

    // Doc tung dong va parse theo ten the
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
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
        else if (line.find("polyline") != std::string::npos)
            group->addElement(parsePolyline(line));
        else if (line.find("<polygon") != std::string::npos)
            group->addElement(parsePolygon(line));
        else if (line.find("<g") != std::string::npos)
        {
            std::string fullContent = readFullGBlock(file, line);
            group->addElement(parseG(fullContent));
        }
    }
    return group;
}
