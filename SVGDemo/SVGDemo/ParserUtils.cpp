#include "stdafx.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include <string>
#include "ParserUtils.h"
#include <iostream>
#include <fstream>


using namespace std;


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