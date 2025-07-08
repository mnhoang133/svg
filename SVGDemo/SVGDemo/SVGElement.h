#ifndef SVGElement_h
#define SVGElement_h
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

class SVGElement 
{
    public:
       virtual void render(Gdiplus::Graphics* graphics) = 0;
       virtual ~SVGElement() = default;
};
#endif 