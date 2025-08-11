#ifndef SVGPOLYGONPARSER_H
#define SVGPOLYGONPARSER_H

#include "IShapeParser.h"

class SVGPolygonParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif 