#include "stdafx.h"
#include "SVGPolyline.h"
#include <gdiplus.h>
#include <cmath>


using namespace Gdiplus;

SVGPolyline::SVGPolyline(const std::vector<svg::Point>& points,
    Color stroke,
    float strokeWidth,
    Color fill)
    : points(points), stroke(stroke), strokeWidth(strokeWidth), fill(fill) {
}

void SVGPolyline::render(Graphics* g) {
    if (points.size() < 2) return;

    std::vector<PointF> gdipPoints;
    for (const auto& p : points) {
        gdipPoints.emplace_back(p.x, p.y);
    }

    GraphicsPath path;
    path.AddLines(gdipPoints.data(), gdipPoints.size());

    // Logic khép hình nếu cần
    bool shouldClose = false;
    if (fill.GetAlpha() > 0 && points.size() >= 3) {
        auto approxEqual = [](float a, float b) {
            return std::abs(a - b) < 0.5f;
            };

        const svg::Point& first = points.front();
        const svg::Point& last = points.back();

        bool diffX = !approxEqual(first.x, last.x);
        bool diffY = !approxEqual(first.y, last.y);

        shouldClose = diffX && diffY;
        if (shouldClose) {
            path.CloseFigure();
        }
    }

    // Fill nếu có
    if (fill.GetAlpha() > 0) {
        SolidBrush brush(fill);
        g->FillPath(&brush, &path);
    }

    // Stroke nếu có
    if (stroke.GetAlpha() > 0) {
        Pen pen(stroke, strokeWidth);
        g->DrawPath(&pen, &path);
    }
}
