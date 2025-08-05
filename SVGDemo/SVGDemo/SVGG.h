#ifndef SVGG_H
#define SVGG_H

#include <string>
#include "Point.h"
#include "SVGElement.h"

#include <vector>

//  Forward declare thay vì include nguyên <gdiplus.h>
namespace Gdiplus {
	class Color;
	class Graphics;
}

class SVGG : public SVGElement {
private:
	std::vector<SVGElement*> children;

	Gdiplus::Color strokeColor;
	float strokeWidth;
	float strokeOpacity;
	Gdiplus::Color fillColor;
	float fillOpacity;
public:
	SVGG();

	void setStyle(Gdiplus::Color stroke, float strokeW, float strokeOp,
		Gdiplus::Color fill, float fillOp);

	void addChild(SVGElement* shape);

	void render(Gdiplus::Graphics* graphics) override;

	~SVGG();

};

#endif
