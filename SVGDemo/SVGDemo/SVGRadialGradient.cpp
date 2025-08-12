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

// SVGRadialGradient methods
Brush* SVGRadialGradient::createBrush(const RectF& bounds) {
    if (stops.empty()) {
        return new SolidBrush(Color(255, 0, 0, 0)); // Black fallback
    }

    // Sort stops by offset
    const_cast<SVGRadialGradient*>(this)->sortStops();

    // For radial gradients, we'll use PathGradientBrush
    // This is a simplified implementation - full radial gradient support is complex in GDI+

    float actualCx, actualCy, actualR;

    if (gradientUnits == "userSpaceOnUse") {
        actualCx = cx;
        actualCy = cy;
        actualR = r;
    }
    else {
        actualCx = bounds.X + cx * bounds.Width;
        actualCy = bounds.Y + cy * bounds.Height;
        actualR = r * std::min<float>(bounds.Width, bounds.Height);
    }

    // Create a circular path
    GraphicsPath path;
    RectF ellipseRect(actualCx - actualR, actualCy - actualR, 2 * actualR, 2 * actualR);
    path.AddEllipse(ellipseRect);

    PathGradientBrush* brush = new PathGradientBrush(&path);

    if (stops.size() >= 2) {
        // Set center color to first stop
        Color centerColor = stops[0].color;
        if (stops[0].opacity < 1.0f) {
            centerColor = Color(static_cast<BYTE>(stops[0].opacity * centerColor.GetA()),
                centerColor.GetR(), centerColor.GetG(), centerColor.GetB());
        }
        brush->SetCenterColor(centerColor);

        // Set surrounding color to last stop
        Color surroundColor = stops[stops.size() - 1].color;
        if (stops[stops.size() - 1].opacity < 1.0f) {
            surroundColor = Color(static_cast<BYTE>(stops[stops.size() - 1].opacity * surroundColor.GetA()),
                surroundColor.GetR(), surroundColor.GetG(), surroundColor.GetB());
        }

        Color surroundColors[] = { surroundColor };
        int count = 1;
        brush->SetSurroundColors(surroundColors, &count);
    }

    return brush;
}