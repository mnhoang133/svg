#ifndef SVGGradient_H
#define SVGGradient_H

#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>



using namespace Gdiplus;

// ==========================
// GradientStop
// ==========================
struct GradientStop {
    float offset;
    Gdiplus::Color color;
    float opacity;

    GradientStop() : offset(0.0f), color(0, 0, 0, 0), opacity(1.0f) {}
    GradientStop(float off, Gdiplus::Color col, float op = 1.0f) : offset(off), color(col), opacity(op) {}
};

// ==========================
// SVGGradient: base class
// ==========================
class SVGGradient {
public:
    std::string id;
    std::string href;
    std::string gradientUnits;          // "userSpaceOnUse" | "objectBoundingBox"
    std::string spreadMethod;           // "pad" | "reflect" | "repeat"
    std::vector<GradientStop> stops;

    // transform có thể lưu raw string hoặc matrix
    Gdiplus::Matrix gradientTransform;

    SVGGradient() : gradientUnits("objectBoundingBox") {}
    virtual ~SVGGradient() {}

    void addStop(const GradientStop& stop);
    void sortStops();
    void inheritFrom(SVGGradient* parent);

    virtual Gdiplus::Brush* createBrush(const Gdiplus::RectF& bounds) = 0;
};


#endif
