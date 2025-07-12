#include "stdafx.h"                      // Luon de dau neu dung precompiled header
#include "SVGRect.h"
#include "Point.h"

using namespace Gdiplus;

// Ham khoi tao SVGRect voi toa do tam, kich thuoc va mau
SVGRect::SVGRect(const svg::Point& center, float width, float height,
    Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), width(width), height(height),
    fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham render de ve hinh chu nhat len Graphics
void SVGRect::render(Gdiplus::Graphics* graphics) {
    Gdiplus::SolidBrush brush(fillColor);            // Doi tuong to mau
    Gdiplus::Pen pen(strokeColor, strokeWidth);      // Doi tuong ve vien

    // Tinh toa do goc tren trai tu tam
    Gdiplus::REAL topLeftX = center.x - width / 2;
    Gdiplus::REAL topLeftY = center.y - height / 2;

    // Ve hinh chu nhat: fill truoc roi draw border
    graphics->FillRectangle(&brush, topLeftX, topLeftY, width, height);
    graphics->DrawRectangle(&pen, topLeftX, topLeftY, width, height);
}
