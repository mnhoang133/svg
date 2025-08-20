#ifndef SVGCIRCLEPARSER_H
#define SVGCIRCLEPARSER_H

#include "IShapeParser.h"

class SVGCircleParser : public IShapeParser 
{
public:
     SVGElement* parse(const std::string& line) const override;
};

#endif
