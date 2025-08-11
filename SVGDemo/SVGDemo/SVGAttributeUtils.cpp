#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include <vector>
#include <string>
#include <sstream>
#include "ParserUtils.h"
#include "SVGAttributeUtils.h"
#include <iostream>
#include <fstream>
#include <regex>


using namespace std;

std::string AttributeParserUtils::mergeAttributes(const std::string& parentTag, const std::string& childTag) {
    std::string merged = childTag;

    std::vector<std::string> attrs = {
        "stroke", "stroke-width", "stroke-opacity",
        "fill", "fill-opacity", "transform"
    };

    for (const std::string& attr : attrs) {
        if (extractAttr(childTag, attr).empty()) {
            std::string val = extractAttr(parentTag, attr);
            if (!val.empty()) {
                size_t pos = merged.find(' ');
                if (pos != std::string::npos) {
                    merged.insert(pos + 1, attr + "=\"" + val + "\" ");
                }
            }
        }
    }

    return merged;
}

// Ham chuyen chuoi SVG mau thanh doi tuong Color
Gdiplus::Color AttributeParserUtils::parseColor(const std::string& s) {
    std::string str = s;
    // Trim đầu cuối và về chữ thường
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (str.empty() || str == "none")
        return Gdiplus::Color(0, 0, 0, 0);  // Trong suốt

    // ===== Hex color =====
    if (str[0] == '#') {
        if (str.length() == 4) {
            // #RGB => #RRGGBB
            int r = std::stoi(std::string(2, str[1]), nullptr, 16);
            int g = std::stoi(std::string(2, str[2]), nullptr, 16);
            int b = std::stoi(std::string(2, str[3]), nullptr, 16);
            return Gdiplus::Color(255, r, g, b);
        }
        else if (str.length() == 7) {
            try {
                unsigned int hex = std::stoul(str.substr(1), nullptr, 16);
                return Gdiplus::Color(255, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
            }
            catch (...) {
                return Gdiplus::Color(255, 0, 0, 0); // fallback
            }
        }
    }

    // ===== rgb(...) hoặc rgba(...) =====
    if (str.find("rgb(") == 0 || str.find("rgba(") == 0) {
        size_t start = str.find("("), end = str.find(")");
        if (start == std::string::npos || end == std::string::npos || end <= start)
            return Gdiplus::Color(255, 0, 0, 0);

        std::string content = str.substr(start + 1, end - start - 1);
        content.erase(remove_if(content.begin(), content.end(), ::isspace), content.end());

        std::istringstream iss(content);
        std::string token;
        int r = 0, g = 0, b = 0, a = 255;

        if (std::getline(iss, token, ',')) r = std::stoi(token);
        if (std::getline(iss, token, ',')) g = std::stoi(token);
        if (std::getline(iss, token, ',')) b = std::stoi(token);
        if (std::getline(iss, token, ',')) {
            float af = std::stof(token);
            if (af < 0.0f) af = 0.0f;
            if (af > 1.0f) af = 1.0f;
            a = static_cast<int>(af * 255.0f);
        }

        // Clamp tay
        r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
        g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
        b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

        return Gdiplus::Color(a, r, g, b);
    }

    // ===== Tên màu phổ biến =====
    if (str == "black")         return Gdiplus::Color(255, 0, 0, 0);
    else if (str == "white")         return Gdiplus::Color(255, 255, 255, 255);
    else if (str == "red")           return Gdiplus::Color(255, 255, 0, 0);
    else if (str == "green")         return Gdiplus::Color(255, 0, 128, 0);
    else if (str == "blue")          return Gdiplus::Color(255, 0, 0, 255);
    else if (str == "yellow")        return Gdiplus::Color(255, 255, 255, 0);
    else if (str == "gray" || str == "grey") return Gdiplus::Color(255, 128, 128, 128);
    else if (str == "cyan")          return Gdiplus::Color(255, 0, 255, 255);
    else if (str == "magenta")       return Gdiplus::Color(255, 255, 0, 255);
    else if (str == "orange")        return Gdiplus::Color(255, 255, 165, 0);
    else if (str == "purple")        return Gdiplus::Color(255, 128, 0, 128);
    else if (str == "brown")         return Gdiplus::Color(255, 165, 42, 42);
    else if (str == "darkslategray") return Gdiplus::Color(255, 47, 79, 79);
    else if (str == "deepskyblue")   return Gdiplus::Color(255, 0, 191, 255);
    else if (str == "navy")          return Gdiplus::Color(255, 0, 0, 128);
    else if (str == "midnightblue")  return Gdiplus::Color(255, 25, 25, 112);
    else if (str == "darkmagenta")   return Gdiplus::Color(255, 139, 0, 139);
    else if (str == "blueviolet")    return Gdiplus::Color(255, 138, 43, 226);
    else if (str == "skyblue")       return Gdiplus::Color(255, 135, 206, 235);

    // fallback
    return Gdiplus::Color(255, 0, 0, 0);
}

// Ham trich thuoc tinh cua the SVG tu chuoi
std::string AttributeParserUtils::extractAttr(const std::string& tag, const std::string& attr) {
    std::string search = attr + "=";
    size_t pos = tag.find(search);

    while (pos != std::string::npos) {
        // Đảm bảo là một attribute thật sự (không phải một phần khác)
        if (pos == 0 || isspace(tag[pos - 1])) {
            size_t start = pos + search.length();
            if (start >= tag.length()) return "";

            char quote = tag[start];
            if (quote != '"' && quote != '\'') return "";

            ++start;
            size_t end = tag.find(quote, start);
            if (end == std::string::npos) return "";

            std::string result = tag.substr(start, end - start);

            return result;
        }

        pos = tag.find(search, pos + 1);
    }

    return "";
}

Gdiplus::Color AttributeParserUtils::parseStyleColor(const std::string& styleStr, const std::string& key, bool isStroke) {
    std::regex regex(key + R"(\s*:\s*([^;]+))");
    std::smatch match;
    if (std::regex_search(styleStr, match, regex)) {
        std::string colorStr = match[1];
        return parseColor(colorStr);
    }

    // Nếu fill thì default là black, nếu stroke thì default là transparent
    return isStroke ? Gdiplus::Color(0, 0, 0, 0) : Gdiplus::Color(0, 0, 0);
}

float AttributeParserUtils::parseStyleFloat(const std::string& styleStr, const std::string& key, float defaultValue) {
    std::regex regex(key + R"(\s*:\s*([^;]+))");
    std::smatch match;
    if (std::regex_search(styleStr, match, regex)) {
        return ParserUtils::safeParseFloat(match[1], defaultValue);
    }
    return defaultValue;
}
