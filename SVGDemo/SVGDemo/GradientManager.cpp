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

// GradientManager methods
void GradientManager::addGradient(const std::string& id, SVGGradient* gradient) {
    if (gradient && !id.empty()) {
        gradients[id] = gradient;
    }
}

SVGGradient* GradientManager::getGradient(const std::string& id) {
    auto it = gradients.find(id);
    return (it != gradients.end()) ? it->second : nullptr;
}

void GradientManager::clear() {
    for (auto& pair : gradients) {
        delete pair.second;
    }
    gradients.clear();
}

Brush* GradientManager::createBrushFromUrl(const std::string& url, const RectF& bounds) {
    // Parse URL format: "url(#gradientId)"
    if (url.find("url(#") == 0 && url.back() == ')') {
        std::string id = url.substr(5, url.length() - 6); // Extract ID between "url(#" and ")"
        SVGGradient* gradient = getGradient(id);
        if (gradient) {
            return gradient->createBrush(bounds);
        }
    }

    // Fallback to solid color
    return new SolidBrush(Color(255, 0, 0, 0));
}