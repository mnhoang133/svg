#ifndef PARSERUTILS_h
#define PARSREULTILS_h


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
}

#endif // !PARSERUTILS_h
