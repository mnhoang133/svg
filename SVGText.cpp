#include "stdafx.h"             // Luon de dau neu dung precompiled headers

#include "SVGText.h"
#include "Point.h"

#include <sstream>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <codecvt> // chuyen wstring sang string


using namespace std;
using namespace Gdiplus;

// Ham khoi tao SVGText voi vi tri, noi dung, co chu va mau chu
SVGText::SVGText(const svg::Point& position,
    const std::wstring& content,
    int fontSize,
    Gdiplus::Color fillColor)
    : position(position), content(content), fontSize(fontSize), fillColor(fillColor) {
}

// Ham render dung Graphics de ve chu len man hinh
void SVGText::render(Graphics* graphics) {
    FontFamily fontFamily(L"Arial");                         // Chon font Arial
    Font font(&fontFamily, (REAL)fontSize, FontStyleRegular, UnitPixel);  // Tao font voi kich thuoc da cho
    SolidBrush brush(fillColor);                             // Brush de to mau chu
    PointF point(position.x, position.y);                    // Vi tri ve chu

    // Ve chu bang DrawString voi noi dung content
    graphics->DrawString(content.c_str(), -1, &font, point, &brush);
}


// chuyen wstring sang utf8 string
static string wstringToUtf8(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}


string SVGText::toSVG() const {
    stringstream ss;

    // bat dau the text
    ss << "<text x=\"" << position.x << "\" y=\"" << position.y << "\"";

    ss << " font-size=\"" << fontSize << "\"";

    ss << " fill=\"rgb("
        << fillColor.GetRed() << ","
        << fillColor.GetGreen() << ","
        << fillColor.GetBlue() << ")\"";

    if (!transform.empty()) {
        ss << " transform=\"" << transform << "\"";
    }

    ss << ">";

    ss << wstringToUtf8(content);

    ss << "</text>";
    return ss.str();
}


