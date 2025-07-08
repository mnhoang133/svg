// SVGParser.cpp
#include "stdafx.h"
#include "SVGParser.h"
#include "SVGCircle.h"
#include "Point.h"
1
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gdiplus.h>

using namespace Gdiplus;

Color parseColor(const std::string& s) {
    if (s.empty() || s == "none")
        return Color(0, 0, 0, 0);  // Transparent

    if (s[0] == '#') {
        unsigned int hex = std::stoul(s.substr(1), nullptr, 16);
        return Color(255, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }

    if (s.find("rgb(") == 0) {
        size_t start = s.find("(");
        size_t end = s.find(")");
        if (start == std::string::npos || end == std::string::npos || end <= start)
            return Color(255, 0, 0, 0);  // fallback

        std::string rgbContent = s.substr(start + 1, end - start - 1);
        rgbContent.erase(remove_if(rgbContent.begin(), rgbContent.end(), ::isspace), rgbContent.end());

        std::istringstream iss(rgbContent);
        std::string token;
        int r = 0, g = 0, b = 0;

        if (std::getline(iss, token, ',')) r = std::stoi(token);
        if (std::getline(iss, token, ',')) g = std::stoi(token);
        if (std::getline(iss, token, ',')) b = std::stoi(token);

        return Color(255, r, g, b);
    }

    return Color(255, 0, 0, 0);  // fallback black
}


std::string extractAttr(const std::string& tag, const std::string& attr) {
    size_t pos = tag.find(attr + "=");
    if (pos == std::string::npos) return "";

    size_t start = pos + attr.length() + 1;
    if (start >= tag.size()) return "";

    char quote = tag[start];
    if (quote != '"' && quote != '\'') return "";

    ++start;
    size_t end = tag.find(quote, start);
    if (end == std::string::npos) return "";

    return tag.substr(start, end - start);
}

std::vector<SVGElement*> SVGParser::parseFile(const std::string& filename) {
    std::vector<SVGElement*> elements;
    std::ifstream file(filename);
    std::string line = "<circle cx=\"200\" cy=\"200\" r=\"100\" fill=\"#ff0000\" stroke=\"#0000ff\" stroke-width=\"5\"/>";

    if (!file.is_open()) {
        std::cerr << "❌ Failed to open file: " << filename << '\n';
        return elements;
    }

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n")); // Trim đầu dòng

        if (line.find("<circle") != std::string::npos) {
            float cx = std::stof(extractAttr(line, "cx"));
            float cy = std::stof(extractAttr(line, "cy"));
            float r = std::stof(extractAttr(line, "r"));

            std::string fillStr = extractAttr(line, "fill");
            if (fillStr.empty()) fillStr = "#ff0000"; // Cho màu đỏ mặc định
            std::string strokeStr = extractAttr(line, "stroke");
            std::string strokeWStr = extractAttr(line, "stroke-width");

            float strokeW = 1.0f;
            if (!strokeWStr.empty()) strokeW = std::stof(strokeWStr);

            Color fillColor = parseColor(fillStr);
            Color strokeColor = parseColor(strokeStr);


            std::string debug = "R: " + std::to_string(fillColor.GetR()) +
                " G: " + std::to_string(fillColor.GetG()) +
                " B: " + std::to_string(fillColor.GetB());

            MessageBoxA(NULL, debug.c_str(), "COLOR", MB_OK);


            SVGElement* circle = new SVGCircle(svg::Point(cx, cy), r, fillColor, strokeColor, strokeW);
            elements.push_back(circle);
        }
    }

    return elements;
}
