#ifndef SVGPATH_H
#define SVGPATH_H

#include <string>
#include "SVGElement.h"

namespace Gdiplus {
    class Color;
    class Graphics;
}

class SVGPath : public SVGElement {
private:
    std::wstring d;
    Gdiplus::Color fill;
    Gdiplus::Color stroke;
    float strokeWidth;
    bool doFill;
    std::string fillUrl;
public:
    SVGPath(const std::wstring& d,
        Gdiplus::Color fill,
        Gdiplus::Color stroke,
        float strokeWidth = 1.0f,
        bool fillEnabled = true);

    void render(Gdiplus::Graphics* graphics) override;
    void setFillUrl(const std::string& url) { fillUrl = url; }
    const std::string& getFillUrl() const { return fillUrl; }
};

#endif // SVGPATH_H

