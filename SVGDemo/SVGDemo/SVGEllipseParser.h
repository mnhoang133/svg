#ifndef SVGELLIPSEPARSER_H
#define SVGELLIPSEPARSER_H

#include "IShapeParser.h"

class SVGEllipseParser: public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif 
