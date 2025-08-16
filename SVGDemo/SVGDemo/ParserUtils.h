#ifndef PARSERUTILS_h
#define PARSREULTILS_h

#include <iostream>
#include <vector>
#include <Gdiplus.h>

namespace ParserUtils 
{
	int clampChannel(int value);
	void logDebug(const std::string& msg);
	float safeParseFloat(const std::string& str, float defaultValue = 0.0f);
	int safeParseInt(const std::string& str, int defaultValue = 0);
	Gdiplus::Color applyOpacity(const Gdiplus::Color& color, float opacity);
	std::string extractOpeningTag(const std::string& block);
	std::string extractInnerContent(const std::string& block);
	std::vector<std::string> extractChildElements(const std::string& innerContent);
	std::string readFullGBlock(std::ifstream& file, const std::string& firstLine);
	inline static std::string trimAttrValue(const std::string& value)
    {
        size_t start = 0;
        size_t end = value.size();

        // bỏ khoảng trắng và dấu " ' ở đầu
        while (start < end && (isspace((unsigned char)value[start]) || value[start] == '"' || value[start] == '\'')) {
            start++;
        }

        // bỏ khoảng trắng và ký tự " ' / > ở cuối
        while (end > start && (isspace((unsigned char)value[end - 1])
            || value[end - 1] == '"'
            || value[end - 1] == '\''
            || value[end - 1] == '/'
            || value[end - 1] == '>')) {
            end--;
        }

        return value.substr(start, end - start);
    }
}

#endif // !PARSERUTILS_h
