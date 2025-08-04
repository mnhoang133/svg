#ifndef SVGPOLYGON_H
#define SVGPOLYGON_H

#include <vector>
#include "Point.h"
#include "SVGElement.h"

// Forward declaration GDI+
namespace Gdiplus {
    class Color;
    class Graphics;
}

class SVGPolygon : public SVGElement {
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
    std::string toSVG() const override;
};

#endif // SVGPOLYGON_H
