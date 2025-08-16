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

void SVGGradient::inheritFrom(SVGGradient* parent) {
    if (!parent) return;

    // copy stops nếu chưa có
    if (this->stops.empty()) {
        this->stops = parent->stops;
    }

    // copy gradientUnits nếu chưa set
    if (this->gradientUnits == "objectBoundingBox" && parent->gradientUnits != "objectBoundingBox") {
        this->gradientUnits = parent->gradientUnits;
    }

    // copy spreadMethod nếu chưa set
    if (this->spreadMethod.empty() && !parent->spreadMethod.empty()) {
        this->spreadMethod = parent->spreadMethod;
    }

    // copy transform nếu mình chưa có (identity)
    if (this->gradientTransform.IsIdentity()) {
        REAL elems[6];
        parent->gradientTransform.GetElements(elems); // elems có 6 giá trị
        this->gradientTransform.SetElements(
            elems[0], elems[1], elems[2],
            elems[3], elems[4], elems[5]
        );
    }


    // chain tiếp nếu cha cũng có href
    if (!parent->href.empty()) {
        std::string refId = parent->href;
        if (!refId.empty() && refId[0] == '#') {
            refId = refId.substr(1);
        }
        SVGGradient* grand = GradientManager::getGradient(refId);
        if (grand) {
            this->inheritFrom(grand);
        }
    }
}
