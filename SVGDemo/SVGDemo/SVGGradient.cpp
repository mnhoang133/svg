#include "stdafx.h"
#include "SVGGradient.h"
#include "SVGAttributeUtils.h"
#include "ParserUtils.h"
#include "GradientManager.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"

#include <algorithm>
#include <cmath>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace AttributeParserUtils;
using namespace ParserUtils;

// Static member initialization
std::map<std::string, SVGGradient*> GradientManager::gradients;

// SVGGradient methods
void SVGGradient::addStop(const GradientStop& stop) {
    stops.push_back(stop);
}

void SVGGradient::sortStops() {
    std::sort(stops.begin(), stops.end(),
        [](const GradientStop& a, const GradientStop& b) {
            return a.offset < b.offset;
        });
}
