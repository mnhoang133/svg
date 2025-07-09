#include "stdafx.h"             //  Luôn để đầu nếu dùng precompiled headers

#include "SVGText.h"
#include "Point.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

SVGText::SVGText(const svg::Point& position,
    const std::wstring& content,
    int fontSize,
    Gdiplus::Color fillColor)
    : position(position), content(content), fontSize(fontSize), fillColor(fillColor) {}

void SVGText::render(Graphics* graphics) {
    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, (REAL)fontSize, FontStyleRegular, UnitPixel);
    SolidBrush brush(fillColor);
    PointF point(position.x, position.y);
    graphics->DrawString(content.c_str(), -1, &font, point, &brush);
}
