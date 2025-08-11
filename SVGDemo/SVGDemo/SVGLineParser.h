#ifndef SVGLINEPARSER_H
#define SVGLINEPARSER_H

#include "IShapeParser.h"

class SVGLineParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif 
