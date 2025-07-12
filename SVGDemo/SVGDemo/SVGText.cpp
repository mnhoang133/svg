#include "stdafx.h"             // Luon de dau neu dung precompiled headers

#include "SVGText.h"
#include "Point.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

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
