#ifndef SVGPATH_H
#define SVGPATH_H

#include <windows.h>
#include <objidl.h>

#include "SVGElement.h"
#include <gdiplus.h>
#include <string>

class SVGPath : public SVGElement {
private:
    std::wstring d; 
    Gdiplus::Color fill;
    Gdiplus::Color stroke;

public:
    SVGPath(const std::wstring& d, Gdiplus::Color fill, Gdiplus::Color stroke);
    void render(Gdiplus::Graphics* graphics) override;
};

#endif
