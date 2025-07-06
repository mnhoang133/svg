#include "SVGPath.h"
#include "stdafx.h"

using namespace Gdiplus;

SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke)
    : d(d), fill(fill), stroke(stroke) {}

void SVGPath::render(Graphics* graphics) {
    GraphicsPath path;
    path.AddLine(10, 10, 100, 50);
    path.AddLine(100, 50, 50, 100);
    path.CloseFigure();

    SolidBrush brush(fill);
    Pen pen(stroke, 2.0f);

    graphics->FillPath(&brush, &path);
    graphics->DrawPath(&pen, &path);
}
