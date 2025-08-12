#ifndef SVGRADIALGRADIENT_H
#define SVGRADIALGRADIENT_H

#include "SVGGradient.h"

// Radial gradient class
class SVGRadialGradient : public SVGGradient {
public:
    float cx, cy, r;       // Tâm và bán kính
    float fx, fy;          // Focal point

    SVGRadialGradient() : cx(0.5f), cy(0.5f), r(0.5f), fx(0.5f), fy(0.5f) {}

    virtual Gdiplus::Brush* createBrush(const Gdiplus::RectF& bounds) override;
};

#endif
