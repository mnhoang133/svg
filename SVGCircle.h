#ifndef SVGCircle_h
#define SVGCircle_h

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Point.h"
#include "SVGElement.h"

class SVGCircle : public SVGElement
{
private:
    svg::Point center;
    float radius;
    Gdiplus::Color fillColor;
    Gdiplus::Color strokeColor;
    float strokeWidth;

public:
    SVGCircle(const svg::Point& center, float radius,
        Gdiplus::Color fillColor,
        Gdiplus::Color strokeColor,
        float strokeWidth);

    void render(Gdiplus::Graphics* graphics) override;
    std::string toSVG() const override;
};

#endif