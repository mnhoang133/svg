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



// Doc file SVG tu ten file va parse cac shape vao mot SVGGroup
SVGGroup* SVGParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;

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

    SVGGroup* group = new SVGGroup({});

    // ========== CASE 1: FILE CÓ 1 DÒNG ==========
    if (lines.size() <= 1) {
        std::regex tagRegex(R"((<rect[\s\S]*?\/?>)|(<circle[\s\S]*?\/?>)|(<ellipse[\s\S]*?\/?>)|(<line[\s\S]*?\/?>)|(<text[\s\S]*?<\/text>)|(<path[\s\S]*?\/?>)|(<polyline[\s\S]*?\/?>)|(<polygon[\s\S]*?\/?>)|(<g[\s\S]*?<\/g>))");
        auto tagsBegin = std::sregex_iterator(svgContent.begin(), svgContent.end(), tagRegex);
        auto tagsEnd = std::sregex_iterator();

        for (auto it = tagsBegin; it != tagsEnd; ++it) {
            std::string shape = it->str();

            if (shape.find("<g") != std::string::npos)
            {
                SVGGParser GP;
                group->addElement(GP.parse(shape));
            }
            else if (shape.find("<circle") != std::string::npos)
            {
                SVGCircleParser PCircle;
                group->addElement(PCircle.parse(line));
            }
            else if (shape.find("<rect") != std::string::npos)
            {
                SVGRectParser PRect;
                group->addElement(PRect.parse(shape));
            }
            else if (shape.find("<ellipse") != std::string::npos)
            {
                SVGEllipseParser PEllipse;
                group->addElement(PEllipse.parse(shape));
            }
            else if (shape.find("<line") != std::string::npos)
            {
                SVGLineParser PLine;
                group->addElement(PLine.parse(shape));
            }
            else if (shape.find("<text") != std::string::npos)
            {
                SVGTextParser PText;
                group->addElement(PText.parse(shape));
            }
            else if (shape.find("<path") != std::string::npos)
            {
                SVGPathParser PPath;
                group->addElement(PPath.parse(shape));
            }
            else if (shape.find("<polyline") != std::string::npos)
            {
                SVGPolylineParser PPolyline;
                group->addElement(PPolyline.parse(shape));
            }
            else if (shape.find("<polygon") != std::string::npos)
            {
                SVGPolygonParser PPolygon;
                group->addElement(PPolygon.parse(shape));
            }
        }
    }
    // ========== CASE 2: FILE NHIỀU DÒNG ==========
    else {
        std::ifstream file3(filename);
        if (!file3.is_open()) return nullptr;

        while (std::getline(file3, line)) {
            if (line.find_first_not_of(" \t\r\n") == std::string::npos)
                continue;

            if (line.find("<g") != std::string::npos) {
                std::string fullGBlock = line;
                int openG = 1;
                while (openG > 0 && std::getline(file3, line)) {
                    fullGBlock += "\n" + line;
                    if (line.find("<g") != std::string::npos) openG++;
                    if (line.find("</g>") != std::string::npos) openG--;
                }
                
                SVGGParser GP;
                group->addElement(GP.parse(fullGBlock));
                //MessageBox(NULL, L"G", L"Tiêu đề", MB_OK);

                continue;
            }

            if (line.find("<circle") != std::string::npos)
            {
                SVGCircleParser PCircle;
                group->addElement(PCircle.parse(line));
                //MessageBox(NULL, L"circle", L"Tiêu đề", MB_OK);
            }
            else if (line.find("<rect") != std::string::npos)
            {
                
                SVGRectParser PRect;
                group->addElement(PRect.parse(line));
                
                //MessageBox(NULL, L"rect", L"Tiêu đề", MB_OK);
            }
            else if (line.find("<ellipse") != std::string::npos)
            {
                SVGEllipseParser PEllipse;
                group->addElement(PEllipse.parse(line));
                //MessageBox(NULL, L"ellipse", L"Tiêu đề", MB_OK);
            }
            else if (line.find("<line") != std::string::npos)
            {
                SVGLineParser PLine;
                group->addElement(PLine.parse(line));
                //MessageBox(NULL, L"line", L"Tiêu đề", MB_OK);
            }
            else if (line.find("<text") != std::string::npos)
            {
                SVGTextParser PText;
                group->addElement(PText.parse(line));
                //MessageBox(NULL, L"text", L"Tiêu đề", MB_OK);

            }
            else if (line.find("<path") != std::string::npos)
            {
                SVGPathParser PPath;
                group->addElement(PPath.parse(line));
                //MessageBox(NULL, L"path", L"Tiêu đề", MB_OK);

            }
            else if (line.find("<polyline") != std::string::npos)
            {
                SVGPolylineParser PPolyline;
                group->addElement(PPolyline.parse(line));
                //MessageBox(NULL, L"polyliene", L"Tiêu đề", MB_OK);
            }
            else if (line.find("<polygon") != std::string::npos)
            {
                SVGPolygonParser PPolygon;
                group->addElement(PPolygon.parse(line));
                //MessageBox(NULL, L"polygon", L"Tiêu đề", MB_OK);

            }
        }
    }

    return group;
}
