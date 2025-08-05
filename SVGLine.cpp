#include "stdafx.h"
#include "SVGLine.h"
#include <sstream>

using namespace std;
using namespace Gdiplus;

// Ham khoi tao duong thang voi 2 diem dau cuoi, mau va do day net ve
SVGLine::SVGLine(const svg::Point& p1,
    const svg::Point& p2,
    Gdiplus::Color strokeColor,
    float strokeWidth)
    : p1(p1), p2(p2), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham ve duong thang
void SVGLine::render(Graphics* graphics) {
    // Tao pen voi mau va do day
    Pen pen(strokeColor, strokeWidth);
    // Ve duong thang tu p1 den p2
    graphics->DrawLine(&pen, p1.x, p1.y, p2.x, p2.y);
}

string SVGLine::toSVG() const {
    stringstream ss;

    // Bat dau the <line ...>
    ss << "<line x1=\"" << p1.x << "\" y1=\"" << p2.y
        << "\" x2=\"" << p1.x << "\" y2=\"" << p2.y << "\"";

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