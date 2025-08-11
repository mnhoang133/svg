#ifndef SVGGPARSER_H
#define SVGGPARSER_H

#include "IShapeParser.h"

class SVGGParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& block) const override;
};

#endif // !SVGCIRCLEPARSER_H
