#ifndef SVGPATH_H
#define SVGPATH_H

#include <string>
#include "SVGElement.h"

//  Forward declaration
namespace Gdiplus {
    class Color;
    class Graphics;
}

class SVGPath : public SVGElement {
private:
    std::wstring d;
    Gdiplus::Color fill;
    Gdiplus::Color stroke;

public:
    SVGPath(const std::wstring& d, Gdiplus::Color fill, Gdiplus::Color stroke);
    void render(Gdiplus::Graphics* graphics) override;
};

#endif // SVGPATH_H
