#ifndef SVGRECTPARSER_H
#define SVGRECTPARSER_H

#include "IShapeParser.h"

class SVGRectParser : public IShapeParser
{
public:
    SVGElement* parse(const std::string& line) const override;
};

#endif // 
