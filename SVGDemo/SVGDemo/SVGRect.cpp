#include "stdafx.h"
#include "SVGRect.h"
#include "Point.h"

using namespace Gdiplus;

SVGRect::SVGRect(const svg::Point& center, float width, float height,
    Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), width(width), height(height),
    fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

void SVGRect::render(Gdiplus::Graphics* graphics) {
    Gdiplus::SolidBrush brush(fillColor);
    Gdiplus::Pen pen(strokeColor, strokeWidth);

    Gdiplus::REAL topLeftX = center.x - width / 2;
    Gdiplus::REAL topLeftY = center.y - height / 2;

    graphics->FillRectangle(&brush, topLeftX, topLeftY, width, height);
    graphics->DrawRectangle(&pen, topLeftX, topLeftY, width, height);
}
