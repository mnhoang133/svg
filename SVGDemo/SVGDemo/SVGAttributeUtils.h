#ifndef SVGATTRIBUTEUTILS_H
#define SVGATTRIBUTEUTILS_H

#include <string>
#include <Gdiplus.h>

namespace AttributeParserUtils {
    std::string extractAttr(const std::string& tag, const std::string& attr);
    Gdiplus::Color parseColor(const std::string& s); 
    Gdiplus::Color parseStyleColor(const std::string& styleStr, const std::string& key, bool isStroke = false);
    float parseStyleFloat(const std::string& styleStr, const std::string& key, float defaultValue = 0.0f);
    std::string mergeAttributes(const std::string& parentTag, const std::string& childTag);
}

#endif 
