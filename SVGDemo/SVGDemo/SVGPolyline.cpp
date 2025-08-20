#include "stdafx.h"                 
#include "SVGPolyline.h"
#include <gdiplus.h>
#include <cmath>

using namespace Gdiplus;

// Ham khoi tao SVGPolyline voi cac diem, stroke, strokeWidth va fill
SVGPolyline::SVGPolyline(const std::vector<svg::Point>& points,
    Color stroke,
    float strokeWidth,
    Color fill)
    : points(points), stroke(stroke), strokeWidth(strokeWidth), fill(fill) {
}

// Ham render de ve polyline len Graphics
void SVGPolyline::render(Graphics* graphics) {
    if (points.size() < 2) return;           // Can it nhat 2 diem moi ve duoc polyline

    Gdiplus::Matrix oldTransform;
    graphics->GetTransform(&oldTransform);

    // Áp dụng transform riêng
    graphics->MultiplyTransform(&transform);

    std::vector<PointF> gdipPoints;
    for (const auto& p : points) {
        gdipPoints.emplace_back(p.x, p.y);   // Chuyen doi sang PointF cua GDI+
    }

    GraphicsPath path;
    path.AddLines(gdipPoints.data(), gdipPoints.size());  // Them tat ca cac line vao path

    // Logic khep hinh neu co fill va so diem >= 3
    bool shouldClose = false;
    if (fill.GetAlpha() > 0 && points.size() >= 3) {
        auto approxEqual = [](float a, float b) {
            return std::abs(a - b) < 0.5f;   // So sanh gan dung do float co the sai so
            };

        const svg::Point& first = points.front();   // Diem dau
        const svg::Point& last = points.back();     // Diem cuoi

        bool diffX = !approxEqual(first.x, last.x);
        bool diffY = !approxEqual(first.y, last.y);

        shouldClose = diffX && diffY;       // Chi dong neu x va y deu khac
        if (shouldClose) {
            path.CloseFigure();             // Dong path lai
        }
    }

    // Neu co fill thi to mau
    if (fill.GetAlpha() > 0) {
        SolidBrush brush(fill);
        graphics->FillPath(&brush, &path);
    }

    // Neu co stroke thi ve vien
    if (stroke.GetAlpha() > 0) {
        Pen pen(stroke, strokeWidth);
        graphics->DrawPath(&pen, &path);
    }

    graphics->SetTransform(&oldTransform);
}
