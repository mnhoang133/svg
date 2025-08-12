#ifndef SVGLINEARGRADIENT_H
#define SVGLINEARGRADIENT_H

#include "SVGGradient.h"

// Linear gradient class
class SVGLinearGradient : public SVGGradient {
public:
    float x1, y1, x2, y2;  // Tọa độ điểm bắt đầu và kết thúc

    SVGLinearGradient() : x1(0.0f), y1(0.0f), x2(1.0f), y2(0.0f) {}

    virtual Gdiplus::Brush* createBrush(const Gdiplus::RectF& bounds) override;
};

#endif