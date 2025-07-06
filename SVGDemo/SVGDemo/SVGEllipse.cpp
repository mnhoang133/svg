#include "stdafx.h"
#include "SVGEllipse.h"
#include "Point.h"

using namespace Gdiplus;

SVGEllipse::SVGEllipse(const svg::Point& center, float radiusX, float radiusY, Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
	: center(center), radiusX(radiusX), radiusY(radiusY), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {}

void SVGEllipse::render(Gdiplus::Graphics* graphics) {
	Gdiplus::SolidBrush brush(fillColor);
	Gdiplus::Pen pen(strokeColor, strokeWidth);
	
	Gdiplus::REAL x = center.x - radiusX;
	Gdiplus::REAL y = center.y - radiusY;
	Gdiplus::REAL width = 2 * radiusX;
	Gdiplus::REAL height = 2 * radiusY;

	graphics->FillEllipse(&brush, x, y, width, height);
	graphics->DrawEllipse(&pen, x, y, width, height);
}