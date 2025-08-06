#include "stdafx.h"             //  Luon de dau tien neu dung precompiled header

#include <sstream>
#include "SVGPath.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>

#include <string>
#include <cctype>

using namespace Gdiplus;

// Ham ho tro: bo qua khoang trang va dau phay
static void skipSeparators(const std::wstring& s, size_t& i) {
    while (i < s.size() && (iswspace(s[i]) || s[i] == L',')) ++i;
}

// Ham ho tro: phan tich mot so float tu chuoi, cap nhat vi tri i
static bool parseNumber(const std::wstring& s, size_t& i, float& out) {
    skipSeparators(s, i);
    if (i >= s.size()) return false;

    size_t start = i;
    bool hasDigit = false;

    if (s[i] == L'+' || s[i] == L'-') ++i;

    while (i < s.size() && iswdigit(s[i])) {
        hasDigit = true;
        ++i;
    }

    if (i < s.size() && s[i] == L'.') {
        ++i;
        while (i < s.size() && iswdigit(s[i])) {
            hasDigit = true;
            ++i;
        }
    }

    if (i < s.size() && (s[i] == L'e' || s[i] == L'E')) {
        ++i;
        if (i < s.size() && (s[i] == L'+' || s[i] == L'-')) ++i;
        bool expDigits = false;
        while (i < s.size() && iswdigit(s[i])) {
            expDigits = true;
            ++i;
        }
        if (!expDigits) return false;
    }

    if (!hasDigit) return false;

    std::wstring numStr = s.substr(start, i - start);
    try {
        out = std::stof(numStr);
    }
    catch (...) {
        return false;
    }
    return true;
}

// Constructor
SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke, float strokeWidth, bool fillEnabled)
    : d(d), fill(fill), stroke(stroke), strokeWidth(strokeWidth), doFill(fillEnabled) {
}

// Render
void SVGPath::render(Graphics* graphics) {
    if (!graphics) return;

    GraphicsPath path;
    const std::wstring& s = d;
    size_t i = 0;

    float x = 0, y = 0;           // current point
    float startX = 0, startY = 0; // subpath start point
    wchar_t currentCmd = 0;

    while (i < s.size()) {
        skipSeparators(s, i);
        if (i >= s.size()) break;

        if (iswalpha(s[i])) {
            currentCmd = s[i];
            ++i;
        }

        switch (currentCmd) {
        case L'M':
        {
            float x1, y1;
            if (!parseNumber(s, i, x1)) break;
            if (!parseNumber(s, i, y1)) break;
            x = x1; y = y1;
            path.StartFigure();
            startX = x;
            startY = y;

            // cac cap sau = lineto
            while (true) {
                size_t save = i;
                float nx, ny;
                if (parseNumber(s, i, nx) && parseNumber(s, i, ny)) {
                    path.AddLine(x, y, nx, ny);
                    x = nx; y = ny;
                }
                else {
                    i = save;
                    break;
                }
            }
            break;
        }
        case L'L':
        {
            while (true) {
                float x1, y1;
                size_t save = i;
                if (parseNumber(s, i, x1) && parseNumber(s, i, y1)) {
                    path.AddLine(x, y, x1, y1);
                    x = x1; y = y1;
                }
                else {
                    i = save;
                    break;
                }
            }
            break;
        }
        case L'H':
        {
            while (true) {
                float x1;
                size_t save = i;
                if (parseNumber(s, i, x1)) {
                    path.AddLine(x, y, x1, y);
                    x = x1;
                }
                else {
                    i = save;
                    break;
                }
            }
            break;
        }
        case L'V':
        {
            while (true) {
                float y1;
                size_t save = i;
                if (parseNumber(s, i, y1)) {
                    path.AddLine(x, y, x, y1);
                    y = y1;
                }
                else {
                    i = save;
                    break;
                }
            }
            break;
        }
        case L'Z': case L'z':
        {
            path.CloseFigure();
            x = startX;
            y = startY;
            break;
        }
        case L'C':
        {
            while (true) {
                size_t save = i;
                float x1, y1, x2, y2, x3, y3;
                if (parseNumber(s, i, x1) &&
                    parseNumber(s, i, y1) &&
                    parseNumber(s, i, x2) &&
                    parseNumber(s, i, y2) &&
                    parseNumber(s, i, x3) &&
                    parseNumber(s, i, y3)) {
                    path.AddBezier(x, y, x1, y1, x2, y2, x3, y3);
                    x = x3; y = y3;
                }
                else {
                    i = save;
                    break;
                }
            }
            break;
        }
        default:
            // Command khong ho tro
            ++i;
            break;
        }
    }
    // Apply transform
    path.Transform(const_cast<Matrix*>(&transform));

    if (doFill) {
        SolidBrush brush(fill);
        graphics->FillPath(&brush, &path);
    }

    Pen pen(stroke, strokeWidth);
    graphics->DrawPath(&pen, &path);
}

