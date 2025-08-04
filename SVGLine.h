#ifndef SVGLINE_H
#define SVGLINE_H

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Point.h"
#include "SVGElement.h"

class SVGLine : public SVGElement
{
private:
    svg::Point p1, p2;
    Gdiplus::Color strokeColor;
    float strokeWidth;

public:
    SVGLine(const svg::Point& p1,
        const svg::Point& p2,
        Gdiplus::Color strokeColor,
        float strokeWidth);

    void render(Gdiplus::Graphics* graphics) override;
    std::string toSVG() const override;
};

#endif
