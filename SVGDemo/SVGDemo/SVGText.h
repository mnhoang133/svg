#ifndef SVGTEXT_H
#define SVGTEXT_H

#include <string>
#include "SVGElement.h"
#include "Point.h"

//  Forward declaration GDI+ types
namespace Gdiplus {
    class Graphics;
    class Color;
}

class SVGText : public SVGElement {
private:
    svg::Point position;
    std::wstring content;
    int fontSize;
    Gdiplus::Color fillColor;

public:
    SVGText(const svg::Point& position,
        const std::wstring& content,
        int fontSize,
        Gdiplus::Color fillColor);

    void render(Gdiplus::Graphics* graphics) override;
};

#endif 
