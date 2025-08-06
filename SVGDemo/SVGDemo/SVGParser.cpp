#include "stdafx.h"
#include "SVGParser.h"
#include "Point.h"

#include <regex>
#include <algorithm>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>
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
    std::string strokeWidthStr = extractAttr(line, "stroke-width");

    float fillOpacity = fillOpStr.empty() ? 1.0f : std::stof(fillOpStr);
    float strokeOpacity = strokeOpStr.empty() ? 1.0f : std::stof(strokeOpStr);
    float strokeWidth = strokeWidthStr.empty() ? 1.0f : std::stof(strokeWidthStr);

    bool fillEnabled = !(fillStr.empty() || fillStr == "none");

    Gdiplus::Color fill = fillEnabled ? applyOpacity(parseColor(fillStr), fillOpacity)
        : Gdiplus::Color(0, 0, 0, 0);

    Gdiplus::Color stroke = strokeStr.empty() || strokeStr == "none"
        ? Gdiplus::Color(0, 0, 0, 0)
        : applyOpacity(parseColor(strokeStr), strokeOpacity);

    auto* path = new SVGPath(std::wstring(d.begin(), d.end()), fill, stroke, strokeWidth, fillEnabled);

    std::string transformStr = extractAttr(line, "transform");
    if (!transformStr.empty()) {
        path->setTransform(std::wstring(transformStr.begin(), transformStr.end()));
    }

    return path;
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
    }

    return group;
}

