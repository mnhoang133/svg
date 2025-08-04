#include "stdafx.h"             //  Luon de dau tien neu dung precompiled header

#include <sstream>
#include "SVGPath.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <codecvt> // de chuyen wstring sang string


using namespace std;
using namespace Gdiplus;

// Ham khoi tao, luu du lieu duong dan SVG va mau fill/stroke
SVGPath::SVGPath(const std::wstring& d, Color fill, Color stroke)
    : d(d), fill(fill), stroke(stroke) {
}

// Ve path len Graphics* su dung GraphicsPath
void SVGPath::render(Graphics* graphics) {
    if (!graphics) return;

    GraphicsPath path;
    std::wistringstream iss(d);     // Stream du lieu path SVG (thuong tu thuoc tinh d)
    std::wstring token;

    wchar_t cmd = 0;                // Lenh SVG dang duoc xu ly (M, L, H, V, Z...)
    float x = 0, y = 0;             // Toa do hien tai

    // Duyet tung token trong chuoi d
    while (iss >> token) {
        if (iswalpha(token[0])) {
            cmd = token[0];         // Neu la ky tu chu cai thi la lenh SVG
        }
        else {
            float x1 = std::stof(token);   // Chuyen doi token thanh so thuc
            float y1;
            if (!(iss >> y1)) break;       // Lay them y1, neu khong co thi dung lai

            // Xu ly theo lenh SVG
            switch (cmd) {
            case 'M': case 'm':            // MoveTo
                path.StartFigure();
                x = x1;
                y = y1;
                break;
            case 'L': case 'l':            // LineTo
                path.AddLine(x, y, x1, y1);
                x = x1;
                y = y1;
                break;
            case 'H': case 'h':            // Horizontal line
                path.AddLine(x, y, x1, y);
                x = x1;
                break;
            case 'V': case 'v':            // Vertical line
                path.AddLine(x, y, x, x1);
                y = x1;
                break;
            }
        }

        // Neu gap lenh Z/z thi dong path lai
        if (cmd == 'Z' || cmd == 'z') {
            path.CloseFigure();
        }
    }

    // Tao brush va pen de to va ve path
    SolidBrush brush(fill);
    Pen pen(stroke, 2.0f);              // Stroke width co dinh la 2.0f

    graphics->FillPath(&brush, &path);  // To mau ben trong path
    graphics->DrawPath(&pen, &path);    // Ve duong vien path
}
// ham chuyen wstring sang utf8 string
static string wstringToUtf8(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

string SVGPath::toSVG() const {
    stringstream ss;

    // bat dau the path
    ss << "<path d=\"" << wstringToUtf8(d) << "\"";

    // fill
    ss << " fill=\"rgb("
        << fill.GetRed() << ","
        << fill.GetGreen() << ","
        << fill.GetBlue() << ")\"";

    // stroke
    ss << " stroke=\"rgb("
        << stroke.GetRed() << ","
        << stroke.GetGreen() << ","
        << stroke.GetBlue() << ")\"";

    // transform
    if (!transform.empty()) {
        ss << " transform=\"" << transform << "\"";
    }

    ss << " />";
    return ss.str();
}