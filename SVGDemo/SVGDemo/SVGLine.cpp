#include "stdafx.h"
#include "SVGLine.h"

using namespace Gdiplus;

SVGLine::SVGLine(const svg::Point& p1,
    const svg::Point& p2,
    Gdiplus::Color strokeColor,
    float strokeWidth)
    : p1(p1), p2(p2), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

void SVGLine::render(Graphics* graphics) {
    Pen pen(strokeColor, strokeWidth);
    graphics->DrawLine(&pen, p1.x, p1.y, p2.x, p2.y);
}
