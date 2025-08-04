#include "stdafx.h"
#include "SVGEllipse.h"
#include "Point.h"

#include <sstream>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;

// Ham khoi tao ellipse voi tam, ban kinh truc x va y, mau to, mau vien, do day net ve
SVGEllipse::SVGEllipse(const svg::Point& center, float radiusX, float radiusY,
    Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), radiusX(radiusX), radiusY(radiusY),
    fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham ve ellipse len man hinh
void SVGEllipse::render(Gdiplus::Graphics* graphics) {
    // Tao brush de to mau ellipse
    SolidBrush brush(fillColor);
    // Tao pen de ve net vien ellipse
    Pen pen(strokeColor, strokeWidth);

    // Tinh vi tri goc tren ben trai va kich thuoc hinh ellipse
    Gdiplus::REAL x = center.x - radiusX;
    Gdiplus::REAL y = center.y - radiusY;
    Gdiplus::REAL width = 2 * radiusX;
    Gdiplus::REAL height = 2 * radiusY;

    // Ve ellipse duoc to mau
    graphics->FillEllipse(&brush, x, y, width, height);
    // Ve net vien ellipse
    graphics->DrawEllipse(&pen, x, y, width, height);
}

string SVGEllipse::toSVG() const {
    stringstream ss;

    // Bat dau the <ellipse ...>
    ss << "<ellipse cx=\"" << center.x << "\" cy=\"" << center.y
        << "\" rx=\"" << radiusX << "\" ry=\"" << radiusY << "\"";

    // fill
    ss << " fill=\"rgb("
        << fillColor.GetRed() << ","
        << fillColor.GetGreen() << ","
        << fillColor.GetBlue() << ")\"";

    // stroke
    ss << " stroke=\"rgb("
        << strokeColor.GetRed() << ","
        << strokeColor.GetGreen() << ","
        << strokeColor.GetBlue() << ")\"";

    // stroke-width
    ss << " stroke-width=\"" << strokeWidth << "\"";

    // Them transform neu co
    if (!transform.empty()) {
        ss << " transform=\"" << transform << "\"";
    }

    ss << " />";
    return ss.str();
}