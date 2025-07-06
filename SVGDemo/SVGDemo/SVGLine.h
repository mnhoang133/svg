#define SVGLine_H
#ifndef SVGLine_h
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Point.h"
#include "SVGElement.h"

class SVGLine : public SVGElement
{
private:
	svg::Point center;
	float radiusX1;
	float radiusY1;
	float radiusX2;
	float radiusY2;
	Gdiplus::Color strokeColor;
	float strokeWidth;
public:
	SVGLine(const svg::Point& center, float radiusX1, float radiusY1,
		float radiusX2, float radiusY2,
		Gdiplus::Color strokeColor, float strokeWidth);


	void render(Gdiplus::Graphics* graphics) override;	
};
#endif
