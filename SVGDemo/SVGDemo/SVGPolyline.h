#ifndef SVGPOLYLINE_H
#define SVGPOLYLINE_H

#include <vector>
#include "Point.h"
#include "SVGElement.h"

namespace Gdiplus {
    class Graphics;
    class Color;
}

class SVGPolyline : public SVGElement {
private:
    std::vector<svg::Point> points;
    Gdiplus::Color stroke;
    Gdiplus::Color fill;
    float strokeWidth;

public:
    SVGPolyline(const std::vector<svg::Point>& points,
        Gdiplus::Color stroke,
        float strokeWidth,
        Gdiplus::Color fill);

    void render(Gdiplus::Graphics* graphics) override;
};

#endif // SVGPOLYLINE_H
