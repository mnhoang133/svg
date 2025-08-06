#include "stdafx.h"
#include "SVGCircle.h"
#include "Point.h"

using namespace Gdiplus;

// Ham khoi tao hinh tron voi tam, ban kinh, mau to, mau vien va do day net ve
SVGCircle::SVGCircle(const svg::Point& center, float radius, Gdiplus::Color fillColor, Gdiplus::Color strokeColor, float strokeWidth)
    : center(center), radius(radius), fillColor(fillColor), strokeColor(strokeColor), strokeWidth(strokeWidth) {
}

// Ham ve hinh tron
    void SVGCircle::render(Gdiplus::Graphics* graphics) {
        // Lưu lại ma trận gốc để restore sau
        Gdiplus::Matrix oldTransform;
        graphics->GetTransform(&oldTransform);

        // Áp dụng transform riêng của SVGCircle
        graphics->MultiplyTransform(&transform);

        // Tạo brush để tô màu bên trong hình tròn
        Gdiplus::SolidBrush brush(fillColor);
        // Tạo pen để vẽ nét viền của hình tròn
        Gdiplus::Pen pen(strokeColor, strokeWidth);

        // Tính tọa độ góc trên bên trái và đường kính
        Gdiplus::REAL x = center.x - radius;
        Gdiplus::REAL y = center.y - radius;
        Gdiplus::REAL diameter = 2 * radius;

        // Vẽ hình tròn được tô màu
        graphics->FillEllipse(&brush, x, y, diameter, diameter);
        // Vẽ nét viền của hình tròn
        graphics->DrawEllipse(&pen, x, y, diameter, diameter);

        // Khôi phục lại transform ban đầu
        graphics->SetTransform(&oldTransform);
    }

