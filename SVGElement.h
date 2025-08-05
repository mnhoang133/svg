#ifndef SVGElement_h
#define SVGElement_h
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>

class SVGElement 
{
    protected:
        std::string transform;
    public:
       virtual void render(Gdiplus::Graphics* graphics) = 0;
       virtual std::string toSVG() const = 0;

       virtual ~SVGElement() = default;

       void setTransform(const std::string& tf) { transform = tf; }
       std::string getTransform() const { return transform; }
       
};
#endif 