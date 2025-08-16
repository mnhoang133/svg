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

// SVGLinearGradient methods
Brush* SVGLinearGradient::createBrush(const RectF& bounds) {
   
    logDebug("createBrush: nigaastops.size()=" + std::to_string(stops.size()));
    for (int i = 0; i < stops.size(); ++i) {
        auto c = stops[i].color;
        logDebug(" stop[" + std::to_string(i) + "] offset=" + std::to_string(stops[i].offset) +
            " rgba=(" + std::to_string(c.GetR()) + "," +
            std::to_string(c.GetG()) + "," +
            std::to_string(c.GetB()) + "," +
            std::to_string(c.GetA()) + ") opacity=" +
            std::to_string(stops[i].opacity));
    }
    if (stops.empty()) {
        return new SolidBrush(Color(255, 0, 0, 0)); // Black fallback
    }



    // Sort stops by offset
    const_cast<SVGLinearGradient*>(this)->sortStops();

    // Calculate actual coordinates
    float actualX1, actualY1, actualX2, actualY2;

    if (gradientUnits == "userSpaceOnUse") {
        actualX1 = x1;
        actualY1 = y1;
        actualX2 = x2;
        actualY2 = y2;
    }
    else {
        // objectBoundingBox - coordinates are percentages
        actualX1 = bounds.X + x1 * bounds.Width;
        actualY1 = bounds.Y + y1 * bounds.Height;
        actualX2 = bounds.X + x2 * bounds.Width;
        actualY2 = bounds.Y + y2 * bounds.Height;
    }

    // Create GDI+ linear gradient brush
    if (stops.size() == 1) {
        Color color = stops[0].color;
        if (stops[0].opacity < 1.0f) {
            color = Color(static_cast<BYTE>(stops[0].opacity * color.GetA()),
                color.GetR(), color.GetG(), color.GetB());
        }
        return new SolidBrush(color);
    }

    // For multiple stops, create a LinearGradientBrush with first and last colors
    Color startColor = stops[0].color;
    Color endColor = stops[stops.size() - 1].color;

    // Apply opacity
    if (stops[0].opacity < 1.0f) {
        startColor = Color(static_cast<BYTE>(stops[0].opacity * startColor.GetA()),
            startColor.GetR(), startColor.GetG(), startColor.GetB());
    }
    if (stops[stops.size() - 1].opacity < 1.0f) {
        endColor = Color(static_cast<BYTE>(stops[stops.size() - 1].opacity * endColor.GetA()),
            endColor.GetR(), endColor.GetG(), endColor.GetB());
    }

    LinearGradientBrush* brush = new LinearGradientBrush(
        PointF(actualX1, actualY1),
        PointF(actualX2, actualY2),
        startColor,
        endColor
    );

    // Set interpolation colors for multiple stops
    if (stops.size() > 2) {
        std::vector<Color> colors;
        std::vector<REAL> positions;

        for (const auto& stop : stops) {
            Color color = stop.color;
            if (stop.opacity < 1.0f) {
                color = Color(static_cast<BYTE>(stop.opacity * color.GetA()),
                    color.GetR(), color.GetG(), color.GetB());
            }
            colors.push_back(color);
            positions.push_back(stop.offset);
        }

        // Before SetInterpolationColors
        if (positions.front() != 0.0f) positions.front() = 0.0f;
        if (positions.back() != 1.0f) positions.back() = 1.0f;

        for (size_t i = 0; i < colors.size(); ++i) {
            logDebug("!!!!!!!!!!!stop[" + std::to_string(i) + "] pos=" + std::to_string(positions[i]) +
                " col=(" + std::to_string(colors[i].GetR()) + "," +
                std::to_string(colors[i].GetG()) + "," +
                std::to_string(colors[i].GetB()) + "," +
                std::to_string(colors[i].GetA()) + ")");
        }

        brush->SetInterpolationColors(&colors[0], &positions[0], colors.size());
    }

    return brush;
}