#ifndef SVGEllipse_h
#define SVGEllipse_h

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Point.h"
#include "SVGElement.h"

class SVGEllipse : public SVGElement
{
private:
	svg::Point center;
	float radiusX;
	float radiusY;
	Gdiplus::Color fillColor;
	Gdiplus::Color strokeColor;
	float strokeWidth;

public:
	SVGEllipse(const svg::Point& center, float radiusX, float radiusY,
		Gdiplus::Color fillColor,
		Gdiplus::Color strokeColor,
		float strokeWidth);

	void render(Gdiplus::Graphics* graphics) override; // Hàm dùng để làm hình này chồng lên hình khác
};

#endif