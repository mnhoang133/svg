#ifndef SVGRect_h
#define SVGRect_h

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Point.h"
#include "SVGElement.h"

class SVGRect : public SVGElement
{
private:
    svg::Point center;
    float width;
    float height;
    Gdiplus::Color fillColor;
    Gdiplus::Color strokeColor;
    float strokeWidth;

public:
    SVGRect(const svg::Point& center, float width, float height,
        Gdiplus::Color fillColor,
        Gdiplus::Color strokeColor,
        float strokeWidth);

    void render(Gdiplus::Graphics* graphics) override;
};

#endif
