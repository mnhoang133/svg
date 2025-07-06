#include "stdafx.h"
#include "SVGRect.h"
#include "Point.h"
#include <cmath>
using namespace Gdiplus;

SVGRect::SVGRect(const svg::Point& center, float radius, Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), radius(radius), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {}

void SVGRect::render(Gdiplus::Graphics* graphics) {
    Gdiplus::SolidBrush brush(fillColor);
    Gdiplus::Pen pen(strokeColor, strokeWidth);

    const int sides = 6; // Số cạnh cần vẽ
    Gdiplus::PointF points[sides]; // Tạo mảng để lưu tạo độ của 6 đỉnh lục giác

    for (int i = 0; i < sides; i++) {
        float angleDeg = 60 * i - 30; // bắt đầu từ góc -30 để nằm ngang
        float angleRad = angleDeg * 3.14159265f / 180.0f;

        points[i].X = center.x + radius * cos(angleRad);
        points[i].Y = center.y + radius * sin(angleRad);
    }

    graphics->FillPolygon(&brush, points, sides);
    graphics->DrawPolygon(&pen, points, sides);
}
