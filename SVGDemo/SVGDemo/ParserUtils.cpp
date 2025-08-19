#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include <string>
#include "ParserUtils.h"
#include <iostream>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


using namespace std;
using namespace Gdiplus;


int  ParserUtils::clampChannel(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

void  ParserUtils::logDebug(const std::string& msg) {
    std::ofstream log("log.txt", std::ios::app);
    log << "[DEBUG] " << msg << "\n";
}


// ====== Helper: Safe parse float/int =======
float  ParserUtils::safeParseFloat(const std::string& str, float defaultValue) {
    try {
        if (str.empty()) return defaultValue;
        return std::stof(str);
    }
    catch (const std::exception&) {
        return defaultValue;
    }
}

int  ParserUtils::safeParseInt(const std::string& str, int defaultValue) {
    try {
        if (str.empty()) return defaultValue;
        return std::stoi(str);
    }
    catch (const std::exception&) {
        return defaultValue;
    }
}

// Ham ap dung do mo cho mau
Gdiplus::Color  ParserUtils::applyOpacity(const Gdiplus::Color& color, float opacity) {
    int alpha = static_cast<int>(opacity * 255.0f);
    return Gdiplus::Color(alpha, color.GetRed(), color.GetGreen(), color.GetBlue());
}

std::string ParserUtils::extractOpeningTag(const std::string& block) {
    size_t start = block.find('<');
    size_t end = block.find('>');
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return "";
    return block.substr(start, end - start + 1);
}

std::string ParserUtils::extractInnerContent(const std::string& block) {
    size_t start = block.find('>');
    size_t end = block.rfind("</g>");
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return "";
    return block.substr(start + 1, end - start - 1);
}

std::vector<std::string> ParserUtils::extractChildElements(const std::string& innerContent) {
    std::vector<std::string> result;
    size_t pos = 0;
    while (pos < innerContent.size()) {
        // Tìm tag mở
        size_t tagStart = innerContent.find('<', pos);
        if (tagStart == std::string::npos) break;

        // Lấy tên tag
        size_t tagNameStart = tagStart + 1;
        while (tagNameStart < innerContent.size() && isspace(innerContent[tagNameStart])) tagNameStart++;
        size_t tagNameEnd = tagNameStart;
        while (tagNameEnd < innerContent.size() && isalpha(innerContent[tagNameEnd])) tagNameEnd++;
        std::string tagName = innerContent.substr(tagNameStart, tagNameEnd - tagNameStart);
        if (tagName.empty()) break;

        // Tìm tag đóng hoặc tự đóng
        size_t tagEnd = innerContent.find('>', tagNameEnd);
        if (tagEnd == std::string::npos) break;

        // Kiểm tra tự đóng
        bool isSelfClose = (innerContent[tagEnd - 1] == '/');
        if (isSelfClose) {
            // Thẻ tự đóng <rect .../> <circle .../> ...
            result.push_back(innerContent.substr(tagStart, tagEnd - tagStart + 1));
            pos = tagEnd + 1;
        }
        else {
            // Thẻ <g> ... </g> hoặc <rect>...</rect>
            std::string closeTag = "</" + tagName + ">";
            size_t closeTagPos = innerContent.find(closeTag, tagEnd);
            if (closeTagPos == std::string::npos) break;
            result.push_back(innerContent.substr(tagStart, closeTagPos + closeTag.size() - tagStart));
            pos = closeTagPos + closeTag.size();
        }
    }

    return result;
}

std::string ParserUtils::readFullGBlock(std::ifstream& file, const std::string& firstLine) {
    std::string block = firstLine + "\n";
    int depth = 1;
    std::string line;
    while (depth > 0 && std::getline(file, line)) {
        block += line + "\n";
        if (line.find("<g") != std::string::npos) depth++;
        if (line.find("</g>") != std::string::npos) depth--;
    }
    return block;
}

inline void skipSeparators(const std::wstring& s, size_t& i) {
    while (i < s.size()) {
        wchar_t c = s[i];
        if (iswspace(c) || c == L',') {
            ++i;
        }
        else {
            break;
        }
    }
}


// Parse flag (0 hoặc 1)
bool ParserUtils::parseFlag(const std::wstring& s, size_t& i, int& flag) {
    skipSeparators(s, i);
    if (i >= s.size() || (s[i] != L'0' && s[i] != L'1')) return false;
    flag = (s[i] == L'1') ? 1 : 0;
    ++i;
    skipSeparators(s, i);
    return true;
}


// Convert SVG arc command to cubic Bezier curves
// Kết quả trả về vector các điểm (chia thành nhóm 4: start + 2 control + end)
void ParserUtils:: arcToBeziers(float x1, float y1, float x2, float y2,
    float rx, float ry, float phi,
    bool largeArc, bool sweep,
    std::vector<PointF>& out) {
    out.clear();

    // Nếu rx, ry = 0 thì coi như line
    if (rx == 0.0f || ry == 0.0f) {
        out.push_back(PointF(x1, y1));
        out.push_back(PointF(x1, y1));
        out.push_back(PointF(x2, y2));
        out.push_back(PointF(x2, y2));
        return;
    }

    // Công thức chuẩn SVG 2.2.7 Elliptical arc implementation notes
    float dx2 = (x1 - x2) / 2.0f;
    float dy2 = (y1 - y2) / 2.0f;
    float cosPhi = cosf(phi);
    float sinPhi = sinf(phi);

    // Step1: transform to prime
    float x1p = cosPhi * dx2 + sinPhi * dy2;
    float y1p = -sinPhi * dx2 + cosPhi * dy2;

    // Step2: correct radii
    float rx_sq = rx * rx;
    float ry_sq = ry * ry;
    float x1p_sq = x1p * x1p;
    float y1p_sq = y1p * y1p;
    float lam = (x1p_sq / rx_sq) + (y1p_sq / ry_sq);
    if (lam > 1.0f) {
        float s = sqrtf(lam);
        rx *= s; ry *= s;
        rx_sq = rx * rx;
        ry_sq = ry * ry;
    }

    // Step3: compute center
    float sign = (largeArc == sweep) ? -1.0f : 1.0f;
    float sq = ((rx_sq * ry_sq) - (rx_sq * y1p_sq) - (ry_sq * x1p_sq)) /
        ((rx_sq * y1p_sq) + (ry_sq * x1p_sq));
    sq = (sq < 0 ? 0 : sq);
    float coef = sign * sqrtf(sq);
    float cxp = coef * (rx * y1p) / ry;
    float cyp = coef * -(ry * x1p) / rx;

    float cx = cosPhi * cxp - sinPhi * cyp + (x1 + x2) / 2.0f;
    float cy = sinPhi * cxp + cosPhi * cyp + (y1 + y2) / 2.0f;

    // Step4: angles
    auto vecAngle = [](float ux, float uy, float vx, float vy) {
        float dot = ux * vx + uy * vy;
        float len = sqrtf((ux * ux + uy * uy) * (vx * vx + vy * vy));
        float val = dot / len;
        if (val < -1.0f) val = -1.0f;
        if (val > 1.0f) val = 1.0f;
        float ang = acosf(val);
        if (ux * vy - uy * vx < 0) ang = -ang;
        return ang;
        };

    float theta1 = vecAngle(1, 0, (x1p - cxp) / rx, (y1p - cyp) / ry);
    float deltaTheta = vecAngle((x1p - cxp) / rx, (y1p - cyp) / ry,
        (-x1p - cxp) / rx, (-y1p - cyp) / ry);

    if (!sweep && deltaTheta > 0) deltaTheta -= 2 * (float)M_PI;
    else if (sweep && deltaTheta < 0) deltaTheta += 2 * (float)M_PI;

    // Step5: split to <=90° segments
    int segments = (int)ceilf(fabsf(deltaTheta / ((float)M_PI / 2.0f)));
    float delta = deltaTheta / segments;
    float t = theta1;

    for (int j = 0; j < segments; ++j) {
        float t1 = t;
        float t2 = t + delta;
        float alpha = (4.0f / 3.0f) * tanf((t2 - t1) / 4.0f);

        float cosT1 = cosf(t1), sinT1 = sinf(t1);
        float cosT2 = cosf(t2), sinT2 = sinf(t2);

        PointF p0(
            cx + rx * (cosPhi * cosT1 - sinPhi * sinT1),
            cy + ry * (sinPhi * cosT1 + cosPhi * sinT1)
        );
        PointF p3(
            cx + rx * (cosPhi * cosT2 - sinPhi * sinT2),
            cy + ry * (sinPhi * cosT2 + cosPhi * sinT2)
        );

        PointF p1(
            p0.X - alpha * (rx * (cosPhi * sinT1 + sinPhi * cosT1)),
            p0.Y - alpha * (ry * (sinPhi * sinT1 - cosPhi * cosT1))
        );
        PointF p2(
            p3.X + alpha * (rx * (cosPhi * sinT2 + sinPhi * cosT2)),
            p3.Y + alpha * (ry * (sinPhi * sinT2 - cosPhi * cosT2))
        );

        if (j == 0) out.push_back(p0);
        out.push_back(p1);
        out.push_back(p2);
        out.push_back(p3);

        t = t2;
    }
}

