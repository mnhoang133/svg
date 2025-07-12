#include "stdafx.h"
#include "SVGCircle.h"
#include "Point.h"

using namespace Gdiplus;

// Ham khoi tao hinh tron voi tam, ban kinh, mau to, mau vien va do day net ve
SVGCircle::SVGCircle(const svg::Point& center, float radius, Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), radius(radius), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham ve hinh tron
void SVGCircle::render(Gdiplus::Graphics* graphics) {
    // Tao brush de to mau ben trong hinh tron
    Gdiplus::SolidBrush brush(fillColor);
    // Tao pen de ve net vien cua hinh tron
    Gdiplus::Pen pen(strokeColor, strokeWidth);

    // Tinh toa do goc tren ben trai va duong kinh
    Gdiplus::REAL x = center.x - radius;
    Gdiplus::REAL y = center.y - radius;
    Gdiplus::REAL diameter = 2 * radius;

    // Ve hinh tron duoc to mau
    graphics->FillEllipse(&brush, x, y, diameter, diameter);
    // Ve net vien cua hinh tron
    graphics->DrawEllipse(&pen, x, y, diameter, diameter);
}
