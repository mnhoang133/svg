#include "stdafx.h"
#include "SVGLine.h"
#include "Point.h"
#include "cmath"

using namespace Gdiplus;

SVGLine::SVGLine(const svg::Point& center, float radiusX1, float radiusY1, float radiusX2, float radiusY2, Gdiplus::Color strokeColor, float strokeWidth)
	: center(center), radiusX1(radiusX1), radiusY1(radiusY1), radiusX2(radiusX2), radiusY2(radiusY2), strokeColor(strokeColor), strokeWidth(strokeWidth) {}

void SVGLine::render(Gdiplus::Graphics* graphics) {
	Gdiplus::Pen pen(strokeColor, strokeWidth);
    Gdiplus::SolidBrush brush(Color(100, 200, 200, 255));

    Gdiplus::REAL x1 = center.x + radiusX1;
    Gdiplus::REAL y1 = center.y + radiusY1;
    Gdiplus::REAL x2 = center.x + radiusX2;
    Gdiplus::REAL y2 = center.y + radiusY2;

    // Tính toạ độ và kích thước hình chữ nhật từ 2 điểm bất kỳ
    Gdiplus::REAL left = min(x1, x2);
    Gdiplus::REAL top = min(y1, y2);
    Gdiplus::REAL width = abs(x2 - x1);
    Gdiplus::REAL height = abs(y2 - y1);

    graphics->FillRectangle(&brush, left, top, width, height);
    graphics->DrawRectangle(&pen, left, top, width, height);
}