#ifndef SVGTEXTPARSER_H
#define SVGTEXTPARSER_H

#include "IShapeParser.h"

class SVGTextParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif // !SVGCIRCLEPARSER_H
