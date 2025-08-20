#include "stdafx.h"                     
#include "SVGPolygon.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Ham khoi tao SVGPolygon voi danh sach diem va mau ve
SVGPolygon::SVGPolygon(const std::vector<svg::Point>& points,
    Gdiplus::Color fillColor,
    Gdiplus::Color strokeColor,
    float strokeWidth)
    : points(points), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham render goi khi can ve polygon len Graphics
void SVGPolygon::render(Gdiplus::Graphics* graphics) {
    if (points.size() < 3) return;       // Khong ve duoc neu it hon 3 diem

    Gdiplus::Matrix oldTransform;
    graphics->GetTransform(&oldTransform);

    // Áp dụng transform riêng
    graphics->MultiplyTransform(&transform);

    std::vector<Gdiplus::PointF> gdiPoints;
    for (const auto& p : points)
        gdiPoints.emplace_back(PointF(p.x, p.y));    // Chuyen sang PointF cua GDI+

    SolidBrush brush(fillColor);                     // Brush to mau polygon
    Pen pen(strokeColor, strokeWidth);               // Pen ve vien polygon

    graphics->FillPolygon(&brush, gdiPoints.data(), gdiPoints.size());  // To mau ben trong
    graphics->DrawPolygon(&pen, gdiPoints.data(), gdiPoints.size());    // Ve duong vien

    graphics->SetTransform(&oldTransform);
}
