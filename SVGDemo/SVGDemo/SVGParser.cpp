#include "stdafx.h"
#include "SVGParser.h"
#include "SVGGParser.h"
#include "SVGCircleParser.h"
#include "SVGEllipseParser.h"
#include "SVGLineParser.h"
#include "SVGRectParser.h"
#include "SVGPolylineParser.h"
#include "SVGPathParser.h"
#include "SVGTextParser.h"
#include "SVGPolygonParser.h"
#include "Point.h"
#include "SVGAttributeUtils.h"
#include "ParserUtils.h"
#include "SVGGradientParser.h"


#include <functional>
#include <map>
#include <regex>
#include <algorithm>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace ParserUtils;
using namespace AttributeParserUtils;

// Trong SVGParser.cpp (hoặc .h nếu bạn muốn khai báo)

// Hàm này nhận 1 đoạn string là 1 tag svg (ví dụ: <circle cx="..." ... />)
// và con trỏ group để thêm element parse được vào
void SVGParser::parseShape(const std::string& shapeContent, SVGGroup* group) {
    // Map tên tag sang hàm parse tương ứng (dùng lambda để gọi parser)
    static const std::map<std::string, std::function<SVGElement* (const std::string&)>> parserMap = {
        {"circle", [](const std::string& s) {
            SVGCircleParser parser;
            return parser.parse(s);
        }},
        {"rect", [](const std::string& s) {
            SVGRectParser parser;
            return parser.parse(s);
        }},
        {"ellipse", [](const std::string& s) {
            SVGEllipseParser parser;
            return parser.parse(s);
        }},
        {"line", [](const std::string& s) {
            SVGLineParser parser;
            return parser.parse(s);
        }},
        {"text", [](const std::string& s) {
            SVGTextParser parser;
            return parser.parse(s);
        }},
        {"path", [](const std::string& s) {
            SVGPathParser parser;
            return parser.parse(s);
        }},
        {"polyline", [](const std::string& s) {
            SVGPolylineParser parser;
            return parser.parse(s);
        }},
        {"polygon", [](const std::string& s) {
            SVGPolygonParser parser;
            return parser.parse(s);
        }},
        {"g", [](const std::string& s) {
            SVGGParser parser;
            return parser.parse(s);
        }}
    };

    logDebug("[PARSE] Checking shape content: " + shapeContent.substr(0, 50) + "...");
    for (const auto& [tag, parseFunc] : parserMap) {
        if (shapeContent.find("<" + tag) != std::string::npos) {
            logDebug("[PARSE] Matched <" + tag + "> → calling " + tag + " parser");
            SVGElement* element = parseFunc(shapeContent);
            if (element != nullptr) {
                logDebug("[PARSE] Parsed <" + tag + "> successfully, adding to group");
                group->addElement(element);
            }
            else  logDebug("[PARSE] Failed to parse <" + tag + ">");
            break;  // tìm thấy tag rồi thì dừng
        }
    }
}


// Doc file SVG tu ten file va parse cac shape vao mot SVGGroup
SVGGroup* SVGParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;

    logDebug("[DEBUG] [PARSE] Enter parseFile, file=" + filename);

    // Đọc file để biết có bao nhiêu dòng
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find_first_not_of(" \t\r\n") != std::string::npos) {
            lines.push_back(line);
        }
    }

    file.close(); // đóng lại để mở lần 2
    std::ifstream file2(filename);
    if (!file2.is_open()) return nullptr;

    // Gom hết nội dung lại thành 1 string (xài cho cả 2 trường hợp)
    std::stringstream buffer;
    buffer << file2.rdbuf();
    std::string svgContent = buffer.str();

    // Xử lý BOM nếu có
    if (svgContent.size() >= 3 &&
        (unsigned char)svgContent[0] == 0xEF &&
        (unsigned char)svgContent[1] == 0xBB &&
        (unsigned char)svgContent[2] == 0xBF)
        svgContent = svgContent.substr(3);

    // ==== Khai báo regex iterator ở đây ====
    std::smatch defsMatch;
    std::regex defsRegex(R"(<defs[^>]*>([\s\S]*?)<\/defs>)");
    if (std::regex_search(svgContent, defsMatch, defsRegex)) {
        logDebug("[DEBUG] [PARSE] Found <defs>, pass to SVGGradientParser");
        std::string defsContent = defsMatch[1].str();
        SVGGradientParser::parseDefs(defsContent);
    }

    std::regex tagRegex(
        R"((<rect[\s\S]*?\/?>)|(<circle[\s\S]*?\/?>)|(<ellipse[\s\S]*?\/?>)|(<line(?!ar)[\s\S]*?\/?>)|(<text[\s\S]*?<\/text>)|(<path[\s\S]*?\/?>)|(<polyline[\s\S]*?\/?>)|(<polygon[\s\S]*?\/?>)|(<g[\s\S]*?<\/g>))"
    );
    auto tagsBegin = std::sregex_iterator(svgContent.begin(), svgContent.end(), tagRegex);
    auto tagsEnd = std::sregex_iterator();

    SVGGroup* group = new SVGGroup({});

    // CASE 1: file 1 dòng dùng regex iterator
    if (lines.size() <= 1) {
        logDebug("[DEBUG] [PARSE] Case 1: Single-line SVG");
        for (auto it = tagsBegin; it != tagsEnd; ++it) {
            std::string shape = it->str();
            SVGParser parser;
            parser.parseShape(shape, group);
        }
    }
    // CASE 2: file nhiều dòng
    else {
        logDebug("[DEBUG] [PARSE] Case 2: Multi-line SVG");
        std::ifstream file3(filename);
        if (!file3.is_open()) return nullptr;

        while (std::getline(file3, line)) {
            if (line.find_first_not_of(" \t\r\n") == std::string::npos)
                continue;

            if (line.find("<g") != std::string::npos) {
                logDebug("[DEBUG] [PARSE] Parse <g> block with nested children");

                std::string fullGBlock = line;
                int openG = 1;
                while (openG > 0 && std::getline(file3, line)) {
                    fullGBlock += "\n" + line;
                    if (line.find("<g") != std::string::npos) openG++;
                    if (line.find("</g>") != std::string::npos) openG--;
                }
                SVGParser parser;
                parser.parseShape(fullGBlock, group);
                continue;
            }
            if (line.find("<linearGradient") != std::string::npos ||
                line.find("<radialGradient") != std::string::npos ||
                line.find("<stop") != std::string::npos) {
                // bỏ qua hoặc để cho SVGGradientParser xử lý
                logDebug("[DEBUG] [PARSE] Gradient found: " + line);
                continue;
            }
            SVGParser parser;
            logDebug("[DEBUG] [PARSE] calling parse Shape");
            parser.parseShape(line, group);
        }
    }

    logDebug("[DEBUG] [PARSE] Exit parseFile, totalChildren=" + std::to_string(group->childCount()));
    return group;
}
