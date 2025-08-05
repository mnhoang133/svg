#include "stdafx.h"             //  Luon de dau tien neu dung precompiled header

#include <sstream>
#include "SVGPath.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>

using namespace Gdiplus;

// Ham khoi tao, luu du lieu duong dan SVG va mau fill/stroke
SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke)
    : d(d), fill(fill), stroke(stroke) {
}

// Ve path len Graphics* su dung GraphicsPath
void SVGPath::render(Graphics* graphics) {
    if (!graphics) return;

    GraphicsPath path;
    std::wistringstream iss(d);
    std::wstring token;
    wchar_t cmd = 0;

    float x = 0, y = 0;           // current point
    float startX = 0, startY = 0; // start point of current subpath

    while (iss >> token) {
        if (iswalpha(token[0])) {
            cmd = token[0];
        }
        else {
            // Put token back into stream and parse based on command
            iss.putback(L' ');
            for (int i = token.size() - 1; i >= 0; --i) {
                iss.putback(token[i]);
            }

            switch (cmd) {
            case 'M':
            {
                float x1, y1;
                iss >> x1;
                if (iss.peek() == ',') iss.get();
                iss >> y1;
                x = x1;
                y = y1;
                path.StartFigure();
                startX = x;
                startY = y;
                break;
            }
            case 'L':
            {
                float x1, y1;
                iss >> x1;
                if (iss.peek() == ',') iss.get();
                iss >> y1;
                path.AddLine(x, y, x1, y1);
                x = x1;
                y = y1;
                break;
            }
            case 'H':
            {
                float x1;
                iss >> x1;
                path.AddLine(x, y, x1, y);
                x = x1;
                break;
            }
            case 'V':
            {
                float y1;
                iss >> y1;
                path.AddLine(x, y, x, y1);
                y = y1;
                break;
            }
            case 'Z': case 'z':
                path.CloseFigure();
                x = startX;
                y = startY;
                break;
            case 'C':
            {
                float x1, y1, x2, y2, x3, y3;
                iss >> x1;
                if (iss.peek() == ',') iss.get();
                iss >> y1;
                iss >> x2;
                if (iss.peek() == ',') iss.get();
                iss >> y2;
                iss >> x3;
                if (iss.peek() == ',') iss.get();
                iss >> y3;

                path.AddBezier(x, y, x1, y1, x2, y2, x3, y3);
                x = x3;
                y = y3;
                break;
            }
            }
        }
    }

    SolidBrush brush(fill);
    Pen pen(stroke, 2.0f);
    graphics->FillPath(&brush, &path);
    graphics->DrawPath(&pen, &path);
}


