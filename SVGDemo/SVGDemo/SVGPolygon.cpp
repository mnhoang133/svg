#include "stdafx.h"
#include "SVGPolygon.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

SVGPolygon::SVGPolygon(const std::vector<svg::Point>& points,
    Gdiplus::Color fillColor,
    Gdiplus::Color strokeColor,
    float strokeWidth)
    : points(points), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

void SVGPolygon::render(Gdiplus::Graphics* graphics) {
    if (points.size() < 3) return;

    std::vector<Gdiplus::PointF> gdiPoints;
    for (const auto& p : points)
        gdiPoints.emplace_back(PointF(p.x, p.y));

    SolidBrush brush(fillColor);
    Pen pen(strokeColor, strokeWidth);

    graphics->FillPolygon(&brush, gdiPoints.data(), gdiPoints.size());
    graphics->DrawPolygon(&pen, gdiPoints.data(), gdiPoints.size());
}
