#ifndef SVGPATHPARSER_H
#define SVGPATHPARSER_H

#include "IShapeParser.h"

class SVGPathParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif // !SVGCIRCLEPARSER_H
