#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <string>
#include <vector>
#include "SVGCircle.h"
#include "SVGEllipse.h"
#include "SVGPolygon.h"
#include "SVGLine.h"
#include "SVGPath.h"
#include "SVGText.h"
#include "SVGRect.h"
#include "SVGPolyline.h"
#include "SVGElement.h"
#include "SVGGroup.h"

class SVGParser {
public:
    static SVGGroup* parseFile(const std::string& filename);

private:
    static std::string extractAttr(const std::string& tag, const std::string& attr);
    static Gdiplus::Color parseColor(const std::string& s);

    // Parse từng loại shape
    static SVGElement* parseCircle(const std::string& line);
    static SVGElement* parseRect(const std::string& line);
    static SVGElement* parseEllipse(const std::string& line);
    static SVGElement* parseLine(const std::string& line);
    static SVGElement* parseText(const std::string& line);
    static SVGElement* parsePath(const std::string& line);
    static SVGElement* parsePolyline(const std::string& line);
    static SVGElement* parsePolygon(const std::string& line);
};

#endif
