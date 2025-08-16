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
    logDebug("createBrush: niggerrstops.size()=" + std::to_string(stops.size()));

    if (stops.empty()) {
        return new SolidBrush(Color(255, 0, 0, 0)); // fallback: trong suốt
    }

    const_cast<SVGRadialGradient*>(this)->sortStops();

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

    int w = (int)bounds.Width;
    int h = (int)bounds.Height;
    Bitmap* bmp = new Bitmap(w, h, PixelFormat32bppARGB);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Tính khoảng cách từ pixel tới tâm
            float dx = x - actualCx;
            float dy = y - actualCy;
            float dist = std::sqrt(dx * dx + dy * dy);

            float t = dist / actualR; // normalize [0..1]

            // clamp
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            // Tìm 2 stops bao quanh t
            const GradientStop* left = &stops.front();
            const GradientStop* right = &stops.back();

            for (size_t i = 1; i < stops.size(); i++) {
                if (t <= stops[i].offset) {
                    left = &stops[i - 1];
                    right = &stops[i];
                    break;
                }
            }

            // Nội suy
            float span = right->offset - left->offset;
            float localT = (span > 0.0f) ? (t - left->offset) / span : 0.0f;

            BYTE r = (BYTE)((1 - localT) * left->color.GetR() + localT * right->color.GetR());
            BYTE g = (BYTE)((1 - localT) * left->color.GetG() + localT * right->color.GetG());
            BYTE b = (BYTE)((1 - localT) * left->color.GetB() + localT * right->color.GetB());
            BYTE a = (BYTE)((1 - localT) * left->color.GetA() + localT * right->color.GetA());

            bmp->SetPixel(x, y, Color(a, r, g, b));
        }
    }

    return new TextureBrush(bmp);
}
