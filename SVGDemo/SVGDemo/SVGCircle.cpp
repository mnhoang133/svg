#include "stdafx.h"
#include "SVGCircle.h"
#include "Point.h"


using namespace Gdiplus;

SVGCircle::SVGCircle(const svg::Point& center, float radius, Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth) : center(center), radius(radius), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {}


void SVGCircle::render(Gdiplus::Graphics* graphics) {
    Gdiplus::SolidBrush brush(fillColor);
    Gdiplus::Pen pen(strokeColor, strokeWidth);

    Gdiplus::REAL x = center.x - radius;
    Gdiplus::REAL y = center.y - radius;
    Gdiplus::REAL diameter = 2 * radius;

    graphics->FillEllipse(&brush, x, y, diameter, diameter);
    graphics->DrawEllipse(&pen, x, y, diameter, diameter);
}
