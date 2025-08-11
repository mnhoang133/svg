#ifndef SVGPOLYLINEPARSER_H
#define SVGPOLYLINEPARSER_H

#include "IShapeParser.h"

class SVGPolylineParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif // !SVGCIRCLEPARSER_H
