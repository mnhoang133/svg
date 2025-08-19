#include "stdafx.h"

#include <sstream>
#include "SVGPath.h"
#include "ParserUtils.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <cctype>
#include "GradientManager.h"
using namespace Gdiplus;
using namespace ParserUtils;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RectF safeBounds(GraphicsPath& path) {
    RectF bounds;
    path.GetBounds(&bounds);

    if (bounds.Width > 0 && bounds.Height > 0) {
        return bounds;
    }

    INT count = path.GetPointCount();
    if (count == 0) {
        return RectF(0, 0, 0, 0);
    }

    std::vector<PointF> pts(count);
    path.GetPathPoints(pts.data(), count);

    float minX = FLT_MAX, minY = FLT_MAX;
    float maxX = -FLT_MAX, maxY = -FLT_MAX;
    for (int i = 0; i < count; i++) {
        if (pts[i].X < minX) minX = pts[i].X;
        if (pts[i].Y < minY) minY = pts[i].Y;
        if (pts[i].X > maxX) maxX = pts[i].X;
        if (pts[i].Y > maxY) maxY = pts[i].Y;
    }

    return RectF(minX, minY, maxX - minX, maxY - minY);
}


static void skipSeparators(const std::wstring& s, size_t& i) {
    while (i < s.size() && (iswspace(s[i]) || s[i] == L',')) ++i;
}

static bool parseNumber(const std::wstring& s, size_t& i, float& out) {
    skipSeparators(s, i);
    if (i >= s.size()) return false;
    size_t start = i;
    bool hasDigit = false;
    if (s[i] == L'+' || s[i] == L'-') ++i;
    while (i < s.size() && iswdigit(s[i])) { hasDigit = true; ++i; }
    if (i < s.size() && s[i] == L'.') {
        ++i;
        while (i < s.size() && iswdigit(s[i])) { hasDigit = true; ++i; }
    }
    if (i < s.size() && (s[i] == L'e' || s[i] == L'E')) {
        ++i;
        if (i < s.size() && (s[i] == L'+' || s[i] == L'-')) ++i;
        bool expDigits = false;
        while (i < s.size() && iswdigit(s[i])) { expDigits = true; ++i; }
        if (!expDigits) return false;
    }
    if (!hasDigit) return false;
    std::wstring numStr = s.substr(start, i - start);
    try { out = std::stof(numStr); }
    catch (...) { return false; }
    return true;
}

SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke, float strokeWidth, bool fillEnabled)
    : d(d), fill(fill), stroke(stroke), strokeWidth(strokeWidth), doFill(fillEnabled) {}

void SVGPath::render(Graphics* graphics) {
    if (!graphics) return;

    GraphicsPath path;
    const std::wstring& s = d;
    size_t i = 0;
    float x = 0, y = 0;
    float startX = 0, startY = 0;
    float prevCtrlX = 0, prevCtrlY = 0;
    float prevQuadX = 0, prevQuadY = 0;
    wchar_t currentCmd = 0;
    bool hasPrevCtrl = false, hasPrevQuad = false;

    while (i < s.size()) {
        skipSeparators(s, i);
        if (i >= s.size()) break;

        if (iswalpha(s[i])) {
            currentCmd = s[i];
            ++i;
        }
        bool relative = iswlower(currentCmd);
        wchar_t cmd = towupper(currentCmd);

        switch (cmd) {
        case L'M': { // move
            float x1, y1;
            if (!parseNumber(s, i, x1) || !parseNumber(s, i, y1)) break;
            if (relative) { x1 += x; y1 += y; }
            x = x1; y = y1;
            path.StartFigure();
            startX = x; startY = y;
            // handle implicit lineto
            while (true) {
                size_t save = i;
                float nx, ny;
                if (parseNumber(s, i, nx) && parseNumber(s, i, ny)) {
                    if (relative) { nx += x; ny += y; }
                    path.AddLine(x, y, nx, ny);
                    x = nx; y = ny;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'L': { // lineto
            while (true) {
                float x1, y1; size_t save = i;
                if (parseNumber(s, i, x1) && parseNumber(s, i, y1)) {
                    if (relative) { x1 += x; y1 += y; }
                    path.AddLine(x, y, x1, y1);
                    x = x1; y = y1;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'H': { // horizontal lineto
            while (true) {
                float x1; size_t save = i;
                if (parseNumber(s, i, x1)) {
                    if (relative) x1 += x;
                    path.AddLine(x, y, x1, y);
                    x = x1;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'V': { // vertical lineto
            while (true) {
                float y1; size_t save = i;
                if (parseNumber(s, i, y1)) {
                    if (relative) y1 += y;
                    path.AddLine(x, y, x, y1);
                    y = y1;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'C': { // cubic bezier
            while (true) {
                size_t save = i;
                float x1, y1, x2, y2, x3, y3;
                if (parseNumber(s, i, x1) && parseNumber(s, i, y1) &&
                    parseNumber(s, i, x2) && parseNumber(s, i, y2) &&
                    parseNumber(s, i, x3) && parseNumber(s, i, y3)) {
                    if (relative) {
                        x1 += x; y1 += y; x2 += x; y2 += y; x3 += x; y3 += y;
                    }
                    path.AddBezier(x, y, x1, y1, x2, y2, x3, y3);
                    prevCtrlX = x2; prevCtrlY = y2; hasPrevCtrl = true;
                    x = x3; y = y3;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'S': { // smooth cubic bezier
            while (true) {
                size_t save = i;
                float x2, y2, x3, y3;
                if (parseNumber(s, i, x2) && parseNumber(s, i, y2) &&
                    parseNumber(s, i, x3) && parseNumber(s, i, y3)) {
                    if (relative) { x2 += x; y2 += y; x3 += x; y3 += y; }
                    float x1 = x, y1 = y;
                    if (hasPrevCtrl && (currentCmd == 'S' || currentCmd == 's' || currentCmd == 'C' || currentCmd == 'c')) {
                        x1 = 2 * x - prevCtrlX;
                        y1 = 2 * y - prevCtrlY;
                    }
                    path.AddBezier(x, y, x1, y1, x2, y2, x3, y3);
                    prevCtrlX = x2; prevCtrlY = y2; hasPrevCtrl = true;
                    x = x3; y = y3;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'Q': { // quadratic bezier
            while (true) {
                size_t save = i;
                float x1, y1, x2, y2;
                if (parseNumber(s, i, x1) && parseNumber(s, i, y1) &&
                    parseNumber(s, i, x2) && parseNumber(s, i, y2)) {
                    if (relative) { x1 += x; y1 += y; x2 += x; y2 += y; }
                    // convert to cubic
                    float cx1 = x + 2.0f / 3.0f * (x1 - x);
                    float cy1 = y + 2.0f / 3.0f * (y1 - y);
                    float cx2 = x2 + 2.0f / 3.0f * (x1 - x2);
                    float cy2 = y2 + 2.0f / 3.0f * (y1 - y2);
                    path.AddBezier(x, y, cx1, cy1, cx2, cy2, x2, y2);
                    prevQuadX = x1; prevQuadY = y1; hasPrevQuad = true;
                    x = x2; y = y2;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'T': { // smooth quadratic bezier
            while (true) {
                size_t save = i;
                float x2, y2;
                if (parseNumber(s, i, x2) && parseNumber(s, i, y2)) {
                    if (relative) { x2 += x; y2 += y; }
                    float x1 = x, y1 = y;
                    if (hasPrevQuad && (currentCmd == 'T' || currentCmd == 't' || currentCmd == 'Q' || currentCmd == 'q')) {
                        x1 = 2 * x - prevQuadX;
                        y1 = 2 * y - prevQuadY;
                    }
                    float cx1 = x + 2.0f / 3.0f * (x1 - x);
                    float cy1 = y + 2.0f / 3.0f * (y1 - y);
                    float cx2 = x2 + 2.0f / 3.0f * (x1 - x2);
                    float cy2 = y2 + 2.0f / 3.0f * (y1 - y2);
                    path.AddBezier(x, y, cx1, cy1, cx2, cy2, x2, y2);
                    prevQuadX = x1; prevQuadY = y1; hasPrevQuad = true;
                    x = x2; y = y2;
                }
                else { i = save; break; }
            }
            break;
        }
        case L'Z': { // close
            path.CloseFigure();
            x = startX; y = startY;
            hasPrevCtrl = false; hasPrevQuad = false;
            break;
        }
        case L'A': { // elliptical arc
            while (true) {
                size_t save = i;
                float rx, ry, xAxisRotation, x2, y2;
                int largeArcFlag, sweepFlag;
                if (parseNumber(s, i, rx) && parseNumber(s, i, ry) &&
                    parseNumber(s, i, xAxisRotation) &&
                    parseFlag(s, i, largeArcFlag) &&
                    parseFlag(s, i, sweepFlag) &&
                    parseNumber(s, i, x2) && parseNumber(s, i, y2)) {

                    if (relative) { x2 += x; y2 += y; }

                    // convert arc to cubic beziers
                    std::vector<PointF> beziers;
                    arcToBeziers(x, y, x2, y2, rx, ry,
                        xAxisRotation * (float)M_PI / 180.0f,
                        largeArcFlag != 0, sweepFlag != 0,
                        beziers);

                    for (size_t k = 0; k + 3 < beziers.size(); k += 3) {
                        path.AddBezier(
                            beziers[k].X, beziers[k].Y,
                            beziers[k + 1].X, beziers[k + 1].Y,
                            beziers[k + 2].X, beziers[k + 2].Y,
                            beziers[k + 3].X, beziers[k + 3].Y
                        );
                    }

                    x = x2; y = y2;
                    hasPrevCtrl = false; hasPrevQuad = false;
                }
                else { i = save; break; }
            }
            break;
        }

        default: // skip unsupported or unknown command
            ++i;
            break;
        }
    }

    path.Transform(const_cast<Matrix*>(&transform));
    if (doFill) {
        if (!fillUrl.empty()) {
            // Tính bounding box để tạo brush gradient
            RectF bounds = safeBounds(path);
            logDebug("[PATH RENDER]: FILLURL: " + fillUrl +
                " Bounds=" + std::to_string(bounds.X) + "," +
                std::to_string(bounds.Y) + "," +
                std::to_string(bounds.Width) + "," +
                std::to_string(bounds.Height));

            Brush* gradientBrush = GradientManager::createBrushFromUrl(fillUrl, bounds);
            if (gradientBrush) {
                graphics->FillPath(gradientBrush, &path);
                delete gradientBrush;
            }
        }
        else {
            SolidBrush brush(fill);
            graphics->FillPath(&brush, &path);
        }
    }
    Pen pen(stroke, strokeWidth);
    graphics->DrawPath(&pen, &path);
}
