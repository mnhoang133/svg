#ifndef SVGELLIPSE_H
#define SVGELLIPSE_H

#include <string>
#include "Point.h"
#include "SVGElement.h"

//  Forward declare thay vì include nguyên <gdiplus.h>
namespace Gdiplus {
	class Color;
	class Graphics;
}

class SVGEllipse : public SVGElement {
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

	void render(Gdiplus::Graphics* graphics) override;
	std::string toSVG() const override;
};

#endif
