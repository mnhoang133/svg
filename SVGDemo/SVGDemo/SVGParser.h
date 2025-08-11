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
#include "SVGG.h"

class SVGParser {
public:
    static SVGGroup* parseFile(const std::string& filename);

private:


    // Parse từng loại shape

    static SVGElement* parseG(const std::string& line);
};

#endif
