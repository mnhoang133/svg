#ifndef SVGTEXT_H
#define SVGTEXT_H

#include <windows.h>
#include <objidl.h>

#include "SVGElement.h"
#include "Point.h"
#include <gdiplus.h>
#include <string>

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
