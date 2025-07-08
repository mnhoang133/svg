#ifndef SVGLine_h
#define SVGLine_h

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include "Point.h"
#include "SVGElement.h"

class SVGPolygon : public SVGElement
{
private:
    std::vector<svg::Point> points;
    Gdiplus::Color fillColor;
    Gdiplus::Color strokeColor;
    float strokeWidth;

public:
    SVGPolygon(const std::vector<svg::Point>& points,
        Gdiplus::Color fillColor,
        Gdiplus::Color strokeColor,
        float strokeWidth);

    void render(Gdiplus::Graphics* graphics) override;
};

#endif
