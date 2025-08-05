#include "stdafx.h"                      // Luon de dau neu dung precompiled header
#include "SVGRect.h"
#include "Point.h"
#include "sstream"

using namespace Gdiplus;
using namespace std;


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

//Transform
string SVGRect::toSVG() const {
    stringstream ss;

    // Tinh x, y tu center va width, height
    float x = center.x - width / 2;
    float y = center.y - height / 2;

    // Bat dau the <rect ...>
    ss << "<rect x=\"" << x << "\" y=\"" << y
        << "\" width=\"" << width << "\" height=\"" << height << "\"";

    // fill="rgb(r,g,b)"
    ss << " fill=\"rgb("
        << fillColor.GetRed() << ","
        << fillColor.GetGreen() << ","
        << fillColor.GetBlue() << ")\"";

    // stroke="rgb(r,g,b)"
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