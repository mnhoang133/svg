#include "stdafx.h"             //  Luôn để đầu tiên nếu dùng precompiled header

#include <sstream>
#include "SVGPath.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke)
    : d(d), fill(fill), stroke(stroke) {}

void SVGPath::render(Graphics* graphics) {
    if (!graphics) return;

    GraphicsPath path;
    std::wistringstream iss(d);
    std::wstring token;

    wchar_t cmd = 0;
    float x = 0, y = 0;

    while (iss >> token) {
        if (iswalpha(token[0])) {
            cmd = token[0];
        }
        else {
            float x1 = std::stof(token);
            float y1;
            if (!(iss >> y1)) break;

            switch (cmd) {
            case 'M': case 'm':
                path.StartFigure();
                x = x1;
                y = y1;
                break;
            case 'L': case 'l':
                path.AddLine(x, y, x1, y1);
                x = x1;
                y = y1;
                break;
            case 'H': case 'h':
                path.AddLine(x, y, x1, y);
                x = x1;
                break;
            case 'V': case 'v':
                path.AddLine(x, y, x, x1);
                y = x1;
                break;
            }
        }

        // Handle Z/z to close path
        if (cmd == 'Z' || cmd == 'z') {
            path.CloseFigure();
        }
    }

    SolidBrush brush(fill);
    Pen pen(stroke, 2.0f);

    graphics->FillPath(&brush, &path);
    graphics->DrawPath(&pen, &path);
}
