#ifndef SVGGradient_H
#define SVGGradient_H

#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Struct để lưu thông tin một gradient stop
struct GradientStop {
    float offset;           // Vị trí từ 0.0 đến 1.0
    Gdiplus::Color color;   // Màu tại vị trí này
    float opacity;          // Độ mờ

    GradientStop() : offset(0.0f), color(0, 0, 0, 0), opacity(1.0f) {}
    GradientStop(float off, Gdiplus::Color col, float op = 1.0f)
        : offset(off), color(col), opacity(op) {}
};

// Base class cho gradient
class SVGGradient {
public:
    std::string id;
    std::vector<GradientStop> stops;
    std::string gradientUnits;  // "userSpaceOnUse" hoặc "objectBoundingBox"
    std::string href;           // Tham chiếu đến gradient khác

    SVGGradient() : gradientUnits("objectBoundingBox") {}
    virtual ~SVGGradient() = default;

    virtual Gdiplus::Brush* createBrush(const Gdiplus::RectF& bounds) = 0;
    void addStop(const GradientStop& stop);
    void sortStops();
};

#endif
